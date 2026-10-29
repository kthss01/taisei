/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * AI bridge. Contract: docs/ai/CONTRACT.md (main repository), schema taisei.ai.obs/1.
 */

#include "ai_bridge.h"

#include "boss.h"
#include "enemy.h"
#include "global.h"
#include "lasers/laser.h"
#include "log.h"
#include "player.h"
#include "plrmodes.h"
#include "projectile.h"
#include "replay/struct.h"
#include "stage.h"
#include "stageinfo.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#define AI_EXPORT EMSCRIPTEN_KEEPALIVE
#else
	#define AI_EXPORT
#endif

#define AI_ABI_VERSION 1
#define AI_OBS_SCHEMA "taisei.ai.obs/1"

#define AI_MAX_BULLETS 4096
#define AI_MAX_LASERS 64
#define AI_MAX_SEGS 4096
#define AI_MAX_ENEMIES 256
#define AI_DEFAULT_MISSING_LIMIT 30

// Field names are the contract. Order only matters inside this file.
#define AI_I32_FIELDS(X) \
	X(schema_ver) X(frame) X(episode_id) X(stage_id) X(stage_type) X(difficulty) X(practice) \
	X(replay_mode) X(skip_mode) X(gameover) X(paused) X(dialog_active) X(seed_lo) X(seed_hi) \
	X(seed_source) X(cheat_flags) X(cheat_events) X(overflow) X(n_bullets) X(n_lasers) X(n_segs) \
	X(n_enemies) X(total_bullets) X(total_lasers) X(total_segs) X(total_enemies) X(owner_state) \
	X(owner_reason) X(owner_events) X(act_applied) X(act_stale) X(act_dup) X(act_missing) \
	X(act_rejected) X(act_last_obs_frame) X(hold_applied) X(act_missing_run) X(act_missing_limit) \
	X(p_alive) X(p_vulnerable) X(p_phase) X(p_inputflags) X(p_focus) X(p_lives) X(p_bombs) \
	X(p_power) X(p_power_stored) X(p_graze) X(p_deathtime) X(p_respawntime) X(p_recoverytime) \
	X(p_bomb_endtime) X(p_bomb_active) X(p_powersurge) X(p_ev_hit) X(p_ev_death) \
	X(p_ev_death_uncounted) X(p_ev_bomb) X(p_continues_used) \
	X(b_present) X(b_attack_type)

#define AI_F32_FIELDS(X) \
	X(p_x) X(p_y) X(p_vx) X(p_vy) X(p_speed_normal) X(p_speed_focus) X(p_hit_radius) \
	X(b_hp) X(b_maxhp) X(b_x) X(b_y) \
	X(cap_ms_last) X(cap_ms_avg) X(cap_ms_max)

#define AI_STATUS_FIELDS(X) \
	X(in_stage) X(paused) X(episode_id) X(last_frame) X(owner_state) X(owner_reason) \
	X(owner_events) X(replay_mode) X(gameover) X(observe)

// X(layout type, C type, name)
#define AI_BULLET_FIELDS(X) \
	X(u32, uint32_t, id) X(f32, float, x) X(f32, float, y) X(f32, float, px) X(f32, float, py) \
	X(f32, float, wx) X(f32, float, wy) X(f32, float, angle) X(f32, float, vx) X(f32, float, vy) \
	X(f32, float, ax) X(f32, float, ay) X(f32, float, ret_re) X(f32, float, ret_im) \
	X(f32, float, attr_re) X(f32, float, attr_im) X(f32, float, attr_x) X(f32, float, attr_y) \
	X(f32, float, attr_exp) X(u32, uint32_t, flags) X(i32, int32_t, age)

#define AI_LASER_FIELDS(X) \
	X(u32, uint32_t, id) X(u32, uint32_t, active) X(i32, int32_t, birth) X(f32, float, timespan) \
	X(f32, float, deathtime) X(f32, float, speed) X(f32, float, width) \
	X(u32, uint32_t, seg_start) X(u32, uint32_t, seg_count)

#define AI_SEG_FIELDS(X) \
	X(f32, float, ax) X(f32, float, ay) X(f32, float, bx) X(f32, float, by) \
	X(f32, float, ra) X(f32, float, rb)

#define AI_ENEMY_FIELDS(X) \
	X(u32, uint32_t, id) X(f32, float, x) X(f32, float, y) X(f32, float, hurt_radius) \
	X(f32, float, hp) X(u32, uint32_t, flags)

enum {
	#define X(n) I_##n,
	AI_I32_FIELDS(X)
	#undef X
	AI_I32_COUNT,
};

enum {
	#define X(n) F_##n,
	AI_F32_FIELDS(X)
	#undef X
	AI_F32_COUNT,
};

enum {
	#define X(n) S_##n,
	AI_STATUS_FIELDS(X)
	#undef X
	AI_STATUS_COUNT,
};

#define AI_DECL_B(t, ct, n) ct n[AI_MAX_BULLETS];
#define AI_DECL_L(t, ct, n) ct n[AI_MAX_LASERS];
#define AI_DECL_S(t, ct, n) ct n[AI_MAX_SEGS];
#define AI_DECL_E(t, ct, n) ct n[AI_MAX_ENEMIES];

// One static block. Offsets are relative to &obs and 4-byte aligned (every element is 4 bytes).
static struct AIObs {
	int32_t i32[AI_I32_COUNT];
	float f32[AI_F32_COUNT];
	int32_t status[AI_STATUS_COUNT];
	struct { AI_BULLET_FIELDS(AI_DECL_B) } bullets;
	struct { AI_LASER_FIELDS(AI_DECL_L) } lasers;
	struct { AI_SEG_FIELDS(AI_DECL_S) } laser_segs;
	struct { AI_ENEMY_FIELDS(AI_DECL_E) } enemies;
} obs;

static struct {
	AIOwner owner;
	AIReason reason;
	int owner_events;
	bool observe;

	bool in_stage;
	bool paused;
	int episode;
	int last_obs_frame;
	bool armed;               // an observation was captured while ai_active
	bool restart_pending;

	struct { bool valid; int ep, target; uint hold, edges; } slot;
	uint cur_hold;            // hold currently in effect
	uint cur_edges;           // edges to fire this tick

	int applied, stale, dup, missing, rejected;
	int missing_run;          // consecutive ticks without an accepted action while ai_active
	int missing_limit;        // allowed consecutive missing ticks before ai_suspended(action_timeout)
	int lim_bullets, lim_lasers, lim_segs, lim_enemies;   // effective capture limits (<= static maxima)
	int last_applied_obs_frame;

	int ev_hit, ev_death, ev_death_uncounted, ev_bomb, cheat_events;

	bool seed_armed, seed_sticky;
	uint64_t seed_value;
	uint64_t cur_seed;
	int cur_seed_source;

	// capture cost (milliseconds, measured inside ai_bridge_capture without the host callback)
	double cap_ms_sum;
	double cap_ms_max;
	int cap_frames;

	char *layout;
	int layout_len;
} ai = {
	.last_obs_frame = -1,
	.last_applied_obs_frame = -1,
	.missing_limit = AI_DEFAULT_MISSING_LIMIT,
	.lim_bullets = AI_MAX_BULLETS,
	.lim_lasers = AI_MAX_LASERS,
	.lim_segs = AI_MAX_SEGS,
	.lim_enemies = AI_MAX_ENEMIES,
};

static const char *const ai_reason_names[AI_REASON_COUNT] = {
	"none", "pause", "focus_lost", "dialog", "stage_end", "restart", "host_request",
	"user_input", "policy_error", "policy_deadline", "observation_invalid", "bridge_error",
	"game_over", "not_playing", "action_timeout",
};

// ---------------------------------------------------------------- status / owner

static void sync_status(void) {
	int32_t *s = obs.status;
	s[S_in_stage] = ai.in_stage;
	s[S_paused] = ai.paused;
	s[S_episode_id] = ai.episode;
	s[S_last_frame] = ai.last_obs_frame;
	s[S_owner_state] = ai.owner;
	s[S_owner_reason] = ai.reason;
	s[S_owner_events] = ai.owner_events;
	s[S_replay_mode] = global.replay.input.replay != NULL;
	s[S_gameover] = global.gameover;
	s[S_observe] = ai.observe;
}

static void clear_pending_input(void) {
	ai.slot.valid = false;
	ai.cur_hold = 0;
	ai.cur_edges = 0;
	ai.armed = false;
	ai.missing_run = 0;
}

static void set_owner(AIOwner owner, AIReason reason) {
	ai.owner = owner;
	ai.reason = reason;
	ai.owner_events++;
	clear_pending_input();
	sync_status();

#ifdef __EMSCRIPTEN__
	EM_ASM({
		if(typeof Module !== 'undefined' && typeof Module['taiseiAIOnEvent'] === 'function') {
			Module['taiseiAIOnEvent'](0, $0, $1);
		}
	}, (int)owner, (int)reason);
#endif
}

static bool is_playing(void) {
	return ai.in_stage && !ai.paused && global.gameover == GAMEOVER_NONE && global.replay.input.replay == NULL;
}

AI_EXPORT int taisei_ai_abi_version(void) {
	return AI_ABI_VERSION;
}

AI_EXPORT void taisei_ai_set_observe(int on) {
	ai.observe = on != 0;
	sync_status();
}

// 0 = ok, 1 = not_playing, 2 = noop
AI_EXPORT int taisei_ai_enable(void) {
	if(ai.owner != AI_OWNER_MANUAL) {
		return 2;
	}

	if(!is_playing()) {
		set_owner(AI_OWNER_SUSPENDED, AI_REASON_NOT_PLAYING);
		return 1;
	}

	set_owner(AI_OWNER_ACTIVE, AI_REASON_HOST_REQUEST);
	return 0;
}

AI_EXPORT int taisei_ai_resume(void) {
	if(ai.owner != AI_OWNER_SUSPENDED) {
		return 2;
	}

	if(!is_playing()) {
		return 1;
	}

	set_owner(AI_OWNER_ACTIVE, AI_REASON_HOST_REQUEST);
	return 0;
}

static AIReason sanitize_reason(int reason) {
	return (reason > 0 && reason < AI_REASON_COUNT) ? (AIReason)reason : AI_REASON_HOST_REQUEST;
}

AI_EXPORT void taisei_ai_suspend(int reason) {
	if(ai.owner == AI_OWNER_ACTIVE) {
		set_owner(AI_OWNER_SUSPENDED, sanitize_reason(reason));
	}
}

AI_EXPORT void taisei_ai_disable(int reason) {
	if(ai.owner != AI_OWNER_MANUAL) {
		set_owner(AI_OWNER_MANUAL, sanitize_reason(reason));
	}
}

AI_EXPORT void taisei_ai_request_restart(void) {
	if(ai.in_stage) {
		ai.restart_pending = true;
	}
}

AI_EXPORT void taisei_ai_set_seed(int lo, int hi, int sticky) {
	ai.seed_value = ((uint64_t)(uint32_t)hi << 32) | (uint32_t)lo;
	ai.seed_armed = true;
	ai.seed_sticky = sticky != 0;
}

AI_EXPORT void taisei_ai_clear_seed(void) {
	ai.seed_armed = false;
	ai.seed_sticky = false;
}

// Consecutive ticks without an accepted action that ai_active tolerates before it drops to
// ai_suspended(action_timeout). 0 = no tolerance. Negative values restore the default.
AI_EXPORT void taisei_ai_set_missing_limit(int limit) {
	ai.missing_limit = limit < 0 ? AI_DEFAULT_MISSING_LIMIT : limit;
}

static int clamp_limit(int v, int max) {
	return (v <= 0 || v > max) ? max : v;
}

// Lowers the number of entries a capture may store (<= the static maxima; <= 0 restores the
// maximum). Used to exercise the overflow path with small, deterministic numbers.
AI_EXPORT void taisei_ai_set_limits(int bullets, int lasers, int segs, int enemies) {
	ai.lim_bullets = clamp_limit(bullets, AI_MAX_BULLETS);
	ai.lim_lasers = clamp_limit(lasers, AI_MAX_LASERS);
	ai.lim_segs = clamp_limit(segs, AI_MAX_SEGS);
	ai.lim_enemies = clamp_limit(enemies, AI_MAX_ENEMIES);
}

// 0 accepted, 1 stale, 2 dup, 3 wrong_episode, 4 not_active, 5 future, 6 no_stage
AI_EXPORT int taisei_ai_submit(int episode, int target_frame, int hold, int edges) {
	if(ai.owner != AI_OWNER_ACTIVE) {
		ai.rejected++;
		return 4;
	}

	if(!ai.in_stage) {
		ai.rejected++;
		return 6;
	}

	if(episode != ai.episode) {
		ai.rejected++;
		return 3;
	}

	if(target_frame < ai.last_obs_frame + 1) {
		ai.stale++;
		return 1;
	}

	if(target_frame > ai.last_obs_frame + 1) {
		ai.rejected++;
		return 5;
	}

	if(ai.slot.valid) {
		ai.dup++;
		return 2;
	}

	ai.slot.valid = true;
	ai.slot.ep = episode;
	ai.slot.target = target_frame;
	ai.slot.hold = (uint)hold & 0x7f;
	ai.slot.edges = (uint)edges & 0x3;
	return 0;
}

// ---------------------------------------------------------------- layout

static void jappend(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static void jappend(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int room = 32768 - ai.layout_len;
	int n = vsnprintf(ai.layout + ai.layout_len, room, fmt, ap);
	va_end(ap);

	if(n > 0 && n < room) {
		ai.layout_len += n;
	}
}

static void build_layout(void) {
	static char buf[32768];
	ai.layout = buf;
	ai.layout_len = 0;

	jappend("{\"schema\":\"" AI_OBS_SCHEMA "\",\"abi\":%d,\"base\":%u,\"size\":%u,",
		AI_ABI_VERSION, (unsigned)(uintptr_t)&obs, (unsigned)sizeof(obs));

	#define BLOCK(NAME, OFFSET, COUNT, LIST, TYPE) do { \
		jappend("\"" NAME "\":{\"type\":\"" TYPE "\",\"offset\":%u,\"count\":%d,\"fields\":{", \
			(unsigned)(OFFSET), (int)(COUNT)); \
		int idx = 0; \
		LIST(JFIELD) \
		jappend("}},"); \
	} while(0)

	#define JFIELD(n) do { jappend("%s\"" #n "\":%d", idx ? "," : "", idx); idx++; } while(0);
	BLOCK("i32", offsetof(struct AIObs, i32), AI_I32_COUNT, AI_I32_FIELDS, "i32");
	BLOCK("f32", offsetof(struct AIObs, f32), AI_F32_COUNT, AI_F32_FIELDS, "f32");
	BLOCK("status", offsetof(struct AIObs, status), AI_STATUS_COUNT, AI_STATUS_FIELDS, "i32");
	#undef JFIELD
	#undef BLOCK

	jappend("\"arrays\":{");

	#define ARRAY(NAME, MEMBER, MAX, COUNTFIELD, LIST) do { \
		jappend("%s\"" NAME "\":{\"max\":%d,\"count_field\":\"" COUNTFIELD "\",\"fields\":{", \
			first_array ? "" : ",", (int)(MAX)); \
		first_array = false; \
		bool first_field = true; \
		LIST(AFIELD) \
		jappend("}}"); \
	} while(0)

	#define AFIELD(t, ct, n) do { \
		jappend("%s\"" #n "\":{\"type\":\"" #t "\",\"offset\":%u}", first_field ? "" : ",", \
			(unsigned)offsetof(struct AIObs, MEMBER_CUR.n)); \
		first_field = false; \
	} while(0);

	bool first_array = true;

	#define MEMBER_CUR bullets
	ARRAY("bullets", bullets, AI_MAX_BULLETS, "n_bullets", AI_BULLET_FIELDS);
	#undef MEMBER_CUR
	#define MEMBER_CUR lasers
	ARRAY("lasers", lasers, AI_MAX_LASERS, "n_lasers", AI_LASER_FIELDS);
	#undef MEMBER_CUR
	#define MEMBER_CUR laser_segs
	ARRAY("laser_segs", laser_segs, AI_MAX_SEGS, "n_segs", AI_SEG_FIELDS);
	#undef MEMBER_CUR
	#define MEMBER_CUR enemies
	ARRAY("enemies", enemies, AI_MAX_ENEMIES, "n_enemies", AI_ENEMY_FIELDS);
	#undef MEMBER_CUR
	#undef AFIELD
	#undef ARRAY

	jappend("},\"enums\":{\"owner_reason\":[");

	for(int i = 0; i < AI_REASON_COUNT; ++i) {
		jappend("%s\"%s\"", i ? "," : "", ai_reason_names[i]);
	}

	jappend("],\"gameover\":{\"NONE\":%d,\"WIN\":%d,\"DEFEAT\":%d,\"ABORT\":%d,\"RESTART\":%d,\"SCORESCREEN\":%d,\"TRANSITIONING\":%d}}}",
		GAMEOVER_NONE, GAMEOVER_WIN, GAMEOVER_DEFEAT, GAMEOVER_ABORT, GAMEOVER_RESTART,
		GAMEOVER_SCORESCREEN, GAMEOVER_TRANSITIONING);
}

AI_EXPORT const char *taisei_ai_layout(void) {
	if(!ai.layout) {
		build_layout();
		sync_status();
	}

	return ai.layout;
}

AI_EXPORT int taisei_ai_layout_size(void) {
	taisei_ai_layout();
	return ai.layout_len;
}

// ---------------------------------------------------------------- capture

static inline float f(real v) { return (float)v; }

static int player_phase(Player *plr, int frame) {
	if(plr->deathtime > frame) {
		return 1;                       // hit_pending
	}

	if(!player_is_alive(plr)) {
		return 2;                       // dead
	}

	if(player_is_bomb_active(plr)) {
		return 4;                       // bombing
	}

	if(player_is_recovering(plr)) {
		return 3;                       // recovering
	}

	return 0;                           // normal
}

static void capture_player(int frame) {
	Player *plr = &global.plr;
	int32_t *v = obs.i32;
	float *w = obs.f32;

	v[I_p_alive] = player_is_alive(plr);
	v[I_p_vulnerable] = player_is_vulnerable(plr);
	v[I_p_phase] = player_phase(plr, frame);
	v[I_p_inputflags] = (int32_t)plr->inputflags;
	v[I_p_focus] = (plr->inputflags & INFLAG_FOCUS) != 0;
	v[I_p_lives] = plr->lives;
	v[I_p_bombs] = plr->bombs;
	v[I_p_power] = player_get_effective_power(plr);
	v[I_p_power_stored] = plr->power_stored;
	v[I_p_graze] = (int32_t)plr->graze;
	v[I_p_deathtime] = plr->deathtime;
	v[I_p_respawntime] = plr->respawntime;
	v[I_p_recoverytime] = plr->recoverytime;
	v[I_p_bomb_endtime] = plr->bomb_endtime;
	v[I_p_bomb_active] = player_is_bomb_active(plr);
	v[I_p_powersurge] = player_is_powersurge_active(plr);
	v[I_p_ev_hit] = ai.ev_hit;
	v[I_p_ev_death] = ai.ev_death;
	v[I_p_ev_death_uncounted] = ai.ev_death_uncounted;
	v[I_p_ev_bomb] = ai.ev_bomb;
	v[I_p_continues_used] = plr->stats.total.continues_used;

	w[F_p_x] = f(re(plr->pos));
	w[F_p_y] = f(im(plr->pos));
	w[F_p_vx] = f(re(plr->velocity));
	w[F_p_vy] = f(im(plr->velocity));
	w[F_p_hit_radius] = 0;

	// PLR_PROP_SPEED depends on the focus flag. Read both values by toggling the flag and
	// restoring it right away (player_property() is a pure lookup).
	uint saved = plr->inputflags;
	plr->inputflags = saved & ~INFLAG_FOCUS;
	w[F_p_speed_normal] = f(player_property(plr, PLR_PROP_SPEED));
	plr->inputflags = saved | INFLAG_FOCUS;
	w[F_p_speed_focus] = f(player_property(plr, PLR_PROP_SPEED));
	plr->inputflags = saved;
}

static void capture_boss(void) {
	Boss *boss = global.boss;
	int32_t *v = obs.i32;
	float *w = obs.f32;

	v[I_b_present] = boss != NULL;
	v[I_b_attack_type] = -1;
	w[F_b_hp] = w[F_b_maxhp] = w[F_b_x] = w[F_b_y] = 0;

	if(boss) {
		w[F_b_x] = f(re(boss->pos));
		w[F_b_y] = f(im(boss->pos));

		if(boss->current) {
			v[I_b_attack_type] = boss->current->type;
			w[F_b_hp] = boss->current->hp;
			w[F_b_maxhp] = boss->current->maxhp;
		}
	}
}

static inline bool bullet_eligible(const Projectile *p) {
	return p->type == PROJ_ENEMY && !(p->flags & (PFLAG_NOCOLLISION | PFLAG_INTERNAL_DEAD));
}

static void put_bullet(int i, const Projectile *p, int frame) {
	obs.bullets.id[i] = p->ent.spawn_id;
	obs.bullets.x[i] = f(re(p->pos));
	obs.bullets.y[i] = f(im(p->pos));
	obs.bullets.px[i] = f(re(p->prevpos));
	obs.bullets.py[i] = f(im(p->prevpos));
	obs.bullets.wx[i] = f(re(p->collision_size));
	obs.bullets.wy[i] = f(im(p->collision_size));
	obs.bullets.angle[i] = (float)(p->angle + M_PI / 2);
	obs.bullets.vx[i] = f(re(p->move.velocity));
	obs.bullets.vy[i] = f(im(p->move.velocity));
	obs.bullets.ax[i] = f(re(p->move.acceleration));
	obs.bullets.ay[i] = f(im(p->move.acceleration));
	obs.bullets.ret_re[i] = f(re(p->move.retention));
	obs.bullets.ret_im[i] = f(im(p->move.retention));
	obs.bullets.attr_re[i] = f(re(p->move.attraction));
	obs.bullets.attr_im[i] = f(im(p->move.attraction));
	obs.bullets.attr_x[i] = f(re(p->move.attraction_point));
	obs.bullets.attr_y[i] = f(im(p->move.attraction_point));
	obs.bullets.attr_exp[i] = f(p->move.attraction_exponent);
	obs.bullets.flags[i] =
		((p->flags & PFLAG_NOMOVE) ? 1u : 0u) |
		((p->flags & PFLAG_INDESTRUCTIBLE) ? 2u : 0u) |
		((p->flags & PFLAG_NOGRAZE) ? 4u : 0u);
	obs.bullets.age[i] = frame - p->birthtime;
}

#define DIST_BUCKETS 256

static int dist_bucket(const Projectile *p) {
	real d = cabs(p->pos - global.plr.pos) / 4.0;
	return d >= DIST_BUCKETS - 1 ? DIST_BUCKETS - 1 : (int)d;
}

static int capture_bullets(int frame, int *out_total) {
	int total = 0;

	for(Projectile *p = global.projs.first; p; p = p->next) {
		if(bullet_eligible(p)) {
			++total;
		}
	}

	*out_total = total;
	int n = 0;

	const int max_b = ai.lim_bullets;

	if(total <= max_b) {
		for(Projectile *p = global.projs.first; p; p = p->next) {
			if(bullet_eligible(p)) {
				put_bullet(n++, p, frame);
			}
		}

		return n;
	}

	// Overflow: keep the bullets nearest to the player (distance buckets of 4 units,
	// list order inside the cutoff bucket).
	int hist[DIST_BUCKETS] = {};

	for(Projectile *p = global.projs.first; p; p = p->next) {
		if(bullet_eligible(p)) {
			hist[dist_bucket(p)]++;
		}
	}

	int cum = 0, cutoff = 0;

	for(; cutoff < DIST_BUCKETS; ++cutoff) {
		if(cum + hist[cutoff] >= max_b) {
			break;
		}

		cum += hist[cutoff];
	}

	int quota = max_b - cum;

	for(Projectile *p = global.projs.first; p && n < max_b; p = p->next) {
		if(!bullet_eligible(p)) {
			continue;
		}

		int b = dist_bucket(p);

		if(b < cutoff || (b == cutoff && quota-- > 0)) {
			put_bullet(n++, p, frame);
		}
	}

	return n;
}

static void capture_lasers(int *out_n, int *out_segs, int *out_total, int *out_total_segs, int *out_flags) {
	int n = 0, nseg = 0, total = 0, total_segs = 0, flags = 0;

	for(Laser *l = global.lasers.first; l; l = l->next) {
		++total;
		int count = 0;
		const LaserSegment *segs = laser_get_segments(l, &count);
		total_segs += count;

		if(n >= ai.lim_lasers) {
			flags |= 2;
			continue;
		}

		int take = count;

		if(nseg + take > ai.lim_segs) {
			take = ai.lim_segs - nseg;
			flags |= 4;
		}

		obs.lasers.id[n] = l->ent.spawn_id;
		obs.lasers.active[n] = laser_is_active(l);
		obs.lasers.birth[n] = l->birthtime;
		obs.lasers.timespan[n] = l->timespan;
		obs.lasers.deathtime[n] = l->deathtime;
		obs.lasers.speed[n] = l->speed;
		obs.lasers.width[n] = l->width;
		obs.lasers.seg_start[n] = nseg;
		obs.lasers.seg_count[n] = take;

		for(int i = 0; i < take; ++i, ++nseg) {
			const LaserSegment *s = &segs[i];
			obs.laser_segs.ax[nseg] = re(s->pos.a);
			obs.laser_segs.ay[nseg] = im(s->pos.a);
			obs.laser_segs.bx[nseg] = re(s->pos.b);
			obs.laser_segs.by[nseg] = im(s->pos.b);
			// Same radius the laser collision uses: max(width / 2 - 4, 2)
			obs.laser_segs.ra[nseg] = max(s->width.a * 0.5f - 4.0f, 2.0f);
			obs.laser_segs.rb[nseg] = max(s->width.b * 0.5f - 4.0f, 2.0f);
		}

		++n;
	}

	*out_n = n;
	*out_segs = nseg;
	*out_total = total;
	*out_total_segs = total_segs;
	*out_flags = flags;
}

static void capture_enemies(int *out_n, int *out_total) {
	int n = 0, total = 0;

	for(Enemy *e = global.enemies.first; e; e = e->next) {
		if(e->flags & EFLAG_KILLED) {
			continue;
		}

		++total;

		if(n >= ai.lim_enemies) {
			continue;
		}

		obs.enemies.id[n] = e->ent.spawn_id;
		obs.enemies.x[n] = f(re(e->pos));
		obs.enemies.y[n] = f(im(e->pos));
		obs.enemies.hurt_radius[n] = enemy_get_hurt_radius(e);
		obs.enemies.hp[n] = e->hp;
		obs.enemies.flags[n] = (uint32_t)e->flags;
		++n;
	}

	*out_n = n;
	*out_total = total;
}

#ifdef __EMSCRIPTEN__
	#define AI_NOW_MS() emscripten_get_now()
#else
	#define AI_NOW_MS() 0.0
#endif

void ai_bridge_capture(void) {
	if(!ai.observe && ai.owner == AI_OWNER_MANUAL) {
		return;
	}

	double t_begin = AI_NOW_MS();
	int frame = global.frames;
	int32_t *v = obs.i32;

	v[I_schema_ver] = 1;
	v[I_frame] = frame;
	v[I_episode_id] = ai.episode;
	v[I_stage_id] = global.stage ? global.stage->id : -1;
	v[I_stage_type] = global.stage ? global.stage->type : -1;
	v[I_difficulty] = global.diff;
	v[I_practice] = global.is_practice_mode;
	v[I_replay_mode] = global.replay.input.replay != NULL;
	v[I_skip_mode] = stage_is_skip_mode();
	v[I_gameover] = global.gameover;
	v[I_paused] = 0;
	v[I_dialog_active] = dialog_is_active(global.dialog);
	v[I_seed_lo] = (int32_t)(uint32_t)ai.cur_seed;
	v[I_seed_hi] = (int32_t)(uint32_t)(ai.cur_seed >> 32);
	v[I_seed_source] = ai.cur_seed_source;

	int cheat = global.plr.iddqd ? 1 : 0;

	if(global.replay.output.replay && (global.replay.output.replay->flags & REPLAY_GFLAG_CHEATS)) {
		cheat |= 2;
	}

	if(stage_is_skip_mode()) {
		cheat |= 4;
	}

#ifdef DEBUG
	cheat |= 8;
#endif

	v[I_cheat_flags] = cheat;
	v[I_cheat_events] = ai.cheat_events;

	capture_player(frame);
	capture_boss();

	int total_b, n_b = capture_bullets(frame, &total_b);
	int n_l, n_s, total_l, total_s, lflags;
	capture_lasers(&n_l, &n_s, &total_l, &total_s, &lflags);
	int n_e, total_e;
	capture_enemies(&n_e, &total_e);

	int overflow = lflags;

	if(total_b > n_b) overflow |= 1;
	if(total_e > n_e) overflow |= 8;

	v[I_overflow] = overflow;
	v[I_n_bullets] = n_b;
	v[I_n_lasers] = n_l;
	v[I_n_segs] = n_s;
	v[I_n_enemies] = n_e;
	v[I_total_bullets] = total_b;
	v[I_total_lasers] = total_l;
	v[I_total_segs] = total_s;
	v[I_total_enemies] = total_e;

	v[I_owner_state] = ai.owner;
	v[I_owner_reason] = ai.reason;
	v[I_owner_events] = ai.owner_events;
	v[I_act_applied] = ai.applied;
	v[I_act_stale] = ai.stale;
	v[I_act_dup] = ai.dup;
	v[I_act_missing] = ai.missing;
	v[I_act_rejected] = ai.rejected;
	v[I_act_last_obs_frame] = ai.last_applied_obs_frame;
	v[I_hold_applied] = (int32_t)global.plr.inputflags;
	v[I_act_missing_run] = ai.missing_run;
	v[I_act_missing_limit] = ai.missing_limit;

	ai.last_obs_frame = frame;

	if(ai.owner == AI_OWNER_ACTIVE) {
		ai.armed = true;
	}

	double cost = AI_NOW_MS() - t_begin;
	ai.cap_ms_sum += cost;
	ai.cap_frames++;
	ai.cap_ms_max = max(ai.cap_ms_max, cost);
	obs.f32[F_cap_ms_last] = (float)cost;
	obs.f32[F_cap_ms_avg] = (float)(ai.cap_ms_sum / ai.cap_frames);
	obs.f32[F_cap_ms_max] = (float)ai.cap_ms_max;

	sync_status();

#ifdef __EMSCRIPTEN__
	EM_ASM({
		if(typeof Module !== 'undefined' && typeof Module['taiseiAIOnFrame'] === 'function') {
			Module['taiseiAIOnFrame']($0, $1);
		}
	}, frame, ai.episode);
#endif
}

// ---------------------------------------------------------------- input

bool ai_bridge_owns_input(void) {
	return ai.owner == AI_OWNER_ACTIVE;
}

uint ai_bridge_hold_flags(void) {
	return ai.cur_hold;
}

void ai_bridge_begin_input(void) {
	if(ai.restart_pending) {
		ai.restart_pending = false;
		stage_finish(GAMEOVER_RESTART);
	}

	if(ai.owner != AI_OWNER_ACTIVE) {
		return;
	}

	ai.cur_edges = 0;

	if(ai.slot.valid && ai.slot.target == global.frames) {
		ai.cur_hold = ai.slot.hold;
		ai.cur_edges = ai.slot.edges;
		ai.last_applied_obs_frame = global.frames - 1;
		ai.applied++;
		ai.missing_run = 0;
		ai.slot.valid = false;
		return;
	}

	if(ai.slot.valid && ai.slot.target < global.frames) {
		ai.stale++;
		ai.slot.valid = false;
	}

	if(ai.armed) {
		ai.missing++;
		ai.missing_run++;

		if(ai.missing_run > ai.missing_limit) {
			// The host stopped answering: drop the held input instead of moving and shooting
			// forever. set_owner() clears the slot, hold and edges; this tick's input then
			// comes from the physical keys again.
			set_owner(AI_OWNER_SUSPENDED, AI_REASON_ACTION_TIMEOUT);
		}
	}
}

void ai_bridge_apply_edges(Player *plr, ReplayState *rpy_out) {
	uint edges = ai.cur_edges;
	ai.cur_edges = 0;

	if(edges & 1) {
		player_event(plr, NULL, rpy_out, EV_PRESS, KEY_BOMB);
	}

	if(edges & 2) {
		player_event(plr, NULL, rpy_out, EV_PRESS, KEY_SPECIAL);
	}
}

static bool is_game_key(int key) {
	switch(key) {
		case KEY_UP: case KEY_DOWN: case KEY_LEFT: case KEY_RIGHT:
		case KEY_FOCUS: case KEY_SHOT: case KEY_BOMB: case KEY_SPECIAL: case KEY_SKIP:
		case KEY_IDDQD: case KEY_POWERUP: case KEY_POWERDOWN: case KEY_HAHAIWIN:
			return true;
		default:
			return false;
	}
}

bool ai_bridge_filter_key(int key, bool is_down) {
	if(ai.owner != AI_OWNER_ACTIVE || !is_game_key(key)) {
		return false;
	}

	if(is_down) {
		// The user takes over. The triggering key event is dropped; the next
		// player_fix_input() rebuilds the held state from the physical keys.
		set_owner(AI_OWNER_MANUAL, AI_REASON_USER_INPUT);
	}

	return true;
}

// ---------------------------------------------------------------- lifecycle hooks

bool ai_bridge_seed_override(uint64_t *seed) {
	if(!ai.seed_armed) {
		return false;
	}

	*seed = ai.seed_value;

	if(!ai.seed_sticky) {
		ai.seed_armed = false;
	}

	return true;
}

void ai_bridge_on_stage_enter(uint64_t seed, int seed_source) {
	ai.episode++;
	ai.in_stage = true;
	ai.paused = false;
	ai.last_obs_frame = -1;
	ai.last_applied_obs_frame = -1;
	ai.restart_pending = false;
	ai.applied = ai.stale = ai.dup = ai.missing = ai.rejected = 0;
	ai.missing_run = 0;
	ai.ev_hit = ai.ev_death = ai.ev_death_uncounted = ai.ev_bomb = ai.cheat_events = 0;
	ai.cur_seed = seed;
	ai.cur_seed_source = seed_source;
	ai.cap_ms_sum = ai.cap_ms_max = 0;
	ai.cap_frames = 0;
	clear_pending_input();

	if(ai.owner == AI_OWNER_ACTIVE) {
		set_owner(AI_OWNER_SUSPENDED, AI_REASON_RESTART);
	}

	sync_status();
}

void ai_bridge_on_stage_leave(void) {
	ai.in_stage = false;
	ai.paused = false;

	if(ai.owner == AI_OWNER_ACTIVE) {
		set_owner(AI_OWNER_SUSPENDED, AI_REASON_STAGE_END);
	}

	sync_status();
}

void ai_bridge_on_stage_finish(int gameover) {
	if(ai.owner == AI_OWNER_ACTIVE) {
		set_owner(
			AI_OWNER_SUSPENDED,
			gameover == GAMEOVER_RESTART ? AI_REASON_RESTART : AI_REASON_STAGE_END
		);
	}

	sync_status();
}

void ai_bridge_on_menu_enter(AIReason reason) {
	ai.paused = true;

	if(ai.owner == AI_OWNER_ACTIVE) {
		set_owner(AI_OWNER_SUSPENDED, reason);
	}

	sync_status();
}

void ai_bridge_on_menu_leave(void) {
	ai.paused = false;
	sync_status();
}

void ai_bridge_on_hit(void) { ai.ev_hit++; }

void ai_bridge_on_death(bool counted) {
	ai.ev_death++;

	if(!counted) {
		ai.ev_death_uncounted++;
	}
}

void ai_bridge_on_bomb(void) { ai.ev_bomb++; }

void ai_bridge_on_cheat(void) { ai.cheat_events++; }
