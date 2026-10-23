/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "emscripten_fetch.h"
#include "emscripten_fetch_public.h"

#include "resindex.h"
#include "rwops/rwops_dummy.h"

#include <dirent.h>
#include <emscripten/em_js.h>
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#include <sys/stat.h>
#include <unistd.h>

#define CACHE_PATH "/persistent/res-cache"
#define MAX_PREFETCH_REQUESTS 4

// 필수 리소스 다운로드가 일시적 오류로 실패했을 때 재시도하는 최대 횟수.
// 404 같은 영구 오류는 재시도하지 않고 즉시 실패 처리한다.
#define MAX_FETCH_ATTEMPTS 3

#define MAKE_CACHED_PATH(_var_name, _content_id) \
	char _var_name[sizeof(CACHE_PATH) + strlen(_content_id) + 1]; \
	snprintf(_var_name, sizeof(_var_name), CACHE_PATH "/%s", (_content_id))

#define MAKE_FETCH_URL(_var_name, _content_id) \
	char _var_name[sizeof(TAISEI_BUILDCONF_DATA_PATH) + strlen(_content_id) + 1]; \
	snprintf(_var_name, sizeof(_var_name), TAISEI_BUILDCONF_DATA_PATH "/%s", (_content_id))

#define FETCH_CONTENT_ID(_fetch) \
	((_fetch)->url + sizeof(TAISEI_BUILDCONF_DATA_PATH))

#define PROP_FETCHIO_POINTER "taisei.fetchio.ptr"

typedef struct FetchFSContext {
	VFSResIndexFSContext resindex_ctx;
	ht_str2ptr_t requests;
	uint prefetch_iter;
} FetchFSContext;

static emscripten_fetch_t *fetch_begin_request(FetchFSContext *ctx, const char *content_id);

EM_JS(void, update_dl_status, (int done, int total), {
	Module["setStatus"](
		done >= total
			? ""
			: "Prefetching resources… (" + done + "/" + total + ")",
	true);
});

// 다운로드를 포기한 리소스를 셸 상태 표시줄에 알린다. 조용히 멈추는 대신
// 사용자와 QA가 어떤 파일이 왜 실패했는지 바로 볼 수 있게 한다.
// 필수 리소스였다면 곧 log_fatal → abort()가 뒤따르고, 셸의 onerror/onAbort가
// 상태 표시를 "Aborted()"로 덮어쓸 수 있다. 셸이 원인을 되살릴 수 있도록
// 마지막 실패 메시지를 Module["lastDownloadError"]에도 남긴다.
EM_JS(void, report_dl_failure, (const char *url, int status), {
	var msg = "Failed to download " + UTF8ToString(url) + " (HTTP status " + status + ")";
	console.error(msg);
	Module["lastDownloadError"] = msg;
	if(typeof Module["setStatus"] === "function") {
		Module["setStatus"]("Error: " + msg, true);
	}
});

// 네트워크 단절(status 0), 타임아웃, 과부하, 서버 오류만 재시도 대상으로 본다.
static bool fetch_status_is_transient(int status) {
	return status == 0 || status == 408 || status == 429 || status >= 500;
}

static void prefetch_next(FetchFSContext *ctx) {
	while(ctx->requests.num_elements_occupied < MAX_PREFETCH_REQUESTS) {
		update_dl_status(ctx->prefetch_iter, resindex_num_file_entries());

		if(ctx->prefetch_iter >= resindex_num_file_entries()) {
			break;
		}

		const char *content_id = resindex_get_file_entry(ctx->prefetch_iter++)->content_id;
		MAKE_CACHED_PATH(cached_path, content_id);

		struct stat statbuf;
		if(stat(cached_path, &statbuf) >= 0) {
			continue;
		}

		fetch_begin_request(ctx, content_id);
	}
}

static void fetch_finalize(emscripten_fetch_t *fetch) {
	FetchFSContext *ctx = fetch->userData;
	assert(ht_get(&ctx->requests, fetch->url, NULL) == fetch);
	ht_unset(&ctx->requests, fetch->url);
	prefetch_next(ctx);
}

static void fetch_onsuccess(emscripten_fetch_t *fetch) {
	fetch_finalize(fetch);

	MAKE_CACHED_PATH(path, FETCH_CONTENT_ID(fetch));

	SDL_IOStream *rw = SDL_IOFromFile(path, "w");
	SDL_WriteIO(rw, fetch->data, fetch->numBytes);
	SDL_CloseIO(rw);

	log_info("Cached %s as %s", fetch->url, path);
}

static void fetch_onerror(emscripten_fetch_t *fetch) {
	log_error(
		"Failed to download %s (status %i: %s)",
		fetch->url, fetch->status, fetch->statusText
	);
	fetch_finalize(fetch);
}

static emscripten_fetch_t *fetch_begin_request(FetchFSContext *ctx, const char *content_id) {
	emscripten_fetch_t *fetch;

	MAKE_FETCH_URL(url, content_id);

	if(ht_lookup(&ctx->requests, url, (void**)&fetch)) {
		return fetch;
	}

	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strlcpy(attr.requestMethod, "GET", sizeof(attr.requestMethod));
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.destinationPath = content_id;
	attr.overriddenMimeType = "application/octet-stream";
	attr.userData = ctx;
	attr.onsuccess = fetch_onsuccess;
	attr.onerror = fetch_onerror;

	fetch = emscripten_fetch(&attr, url);
	ht_set(&ctx->requests, url, fetch);

	log_info("Downloading %s", url);
	return fetch;
}

static void fetch_wait(emscripten_fetch_t *fetch) {
	// cursed
	while(fetch->readyState < 4 /* DONE */) {
		emscripten_sleep(10);
	}
}

static void fetch_io_cleanup(void *userdata, void *value) {
	emscripten_fetch_close(value);
}

static SDL_IOStream *vfs_fetch_open(VFSResIndexFSContext *resindex_ctx, const char *content_id, VFSOpenMode mode) {
	assert(!(mode & VFS_MODE_WRITE));

	FetchFSContext *ctx = UNION_CAST(VFSResIndexFSContext*, FetchFSContext*, resindex_ctx);
	emscripten_fetch_t *fetch;

	MAKE_CACHED_PATH(cached_path, content_id);

	SDL_IOStream *cached_rw = SDL_IOFromFile(cached_path, "r");
	if(cached_rw) {
		return cached_rw;
	}

	// 이전에는 200이 아닐 때 무한히 재요청해서, 파일 누락이나 404 시 로딩이
	// 영원히 멈추고 서버에 요청이 쏟아졌다. 일시적 오류만 제한 횟수로 재시도하고
	// 그 외에는 VFS 오류로 실패를 돌려준다.
	for(int attempt = 1;; ++attempt) {
		fetch = fetch_begin_request(ctx, content_id);
		fetch_wait(fetch);

		if(fetch->status == 200) {
			break;
		}

		int status = fetch->status;
		emscripten_fetch_close(fetch);

		if(attempt < MAX_FETCH_ATTEMPTS && fetch_status_is_transient(status)) {
			log_warn("Retrying %s (attempt %i of %i)", content_id, attempt + 1, MAX_FETCH_ATTEMPTS);
			emscripten_sleep(500 * attempt);
			continue;
		}

		MAKE_FETCH_URL(url, content_id);
		log_error("Giving up on %s after %i attempt(s); last HTTP status %i", url, attempt, status);
		report_dl_failure(url, status);
		vfs_set_error("Failed to download %s (HTTP status %i)", url, status);
		return NULL;
	}

	SDL_IOStream *memrw = NOT_NULL(SDL_IOFromConstMem(fetch->data, fetch->numBytes));
	SDL_SetPointerPropertyWithCleanup(SDL_GetIOProperties(memrw),
		PROP_FETCHIO_POINTER, fetch, fetch_io_cleanup, NULL);

	return memrw;
}

static void vfs_fetch_free(VFSResIndexFSContext *resindex_ctx) {
	FetchFSContext *ctx = UNION_CAST(VFSResIndexFSContext*, FetchFSContext*, resindex_ctx);
	ht_destroy(&ctx->requests);
	mem_free(ctx);
}

static void evict_stale_cache(void) {
	ht_str2int_t idset;
	ht_create(&idset);

	int num_files = resindex_num_file_entries();
	for(int i = 0; i < num_files; ++i) {
		const RIdxFileEntry *fentry = NOT_NULL(resindex_get_file_entry(i));
		ht_set(&idset, fentry->content_id, 1);
	}

	char path[PATH_MAX] = CACHE_PATH "/";
	char *fname = path + sizeof(CACHE_PATH);

	DIR *dp = opendir(CACHE_PATH);

	for(struct dirent *d; (d = readdir(dp));) {
		if(*d->d_name == '.') {
			continue;
		}

		if(!ht_lookup(&idset, d->d_name, NULL)) {
			strlcpy(fname, d->d_name, sizeof(path) - sizeof(CACHE_PATH));
			log_info("Removing stale cache entry %s", path);
			unlink(path);
		}
	}

	closedir(dp);
	ht_destroy(&idset);
}

VFSNode *vfs_fetch_create(void) {
	auto ctx = ALLOC(FetchFSContext, {
		.resindex_ctx.procs = {
			.open = vfs_fetch_open,
			.free = vfs_fetch_free,
		},
	});
	ht_create(&ctx->requests);
	mkdir(CACHE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	VFSNode *n = vfs_resindex_create(&ctx->resindex_ctx);
	evict_stale_cache();
	prefetch_next(ctx);
	return n;
}

bool vfs_mount_fetchfs(const char *mountpoint) {
	return vfs_mount_or_decref(vfs_root, mountpoint, vfs_fetch_create());
}
