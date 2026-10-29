/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "common.h"

#include "audio/audio.h"
#include "charselect.h"
#include "credits.h"
#include "cutscenes/cutscene.h"
#include "difficultyselect.h"
#include "global.h"
#include "i18n/i18n.h"
#include "mainmenu.h"
#include "menu.h"
#include "progress.h"
#include "replay/state.h"
#include "replay/struct.h"
#include "resource/font.h"
#include "savereplay.h"
#include "stage.h"
#include "video.h"

typedef struct StartGameContext {
	StageInfo *restart_stage;
	StageInfo *current_stage;
	MenuData *diff_menu;
	MenuData *char_menu;
	Replay replay;
	Difficulty difficulty;
	ResourceGroup rg;
} StartGameContext;

static void start_game_do_pick_character(CallChainResult ccr);
static void start_game_do_enter_stage(CallChainResult ccr);
static void start_game_do_leave_stage(CallChainResult ccr);
static void start_game_do_show_ending(CallChainResult ccr);
static void start_game_do_show_credits(CallChainResult ccr);
static void start_game_do_cleanup(CallChainResult ccr);

static void start_game_internal(MenuData *menu, StageInfo *info, bool difficulty_menu) {
	auto ctx = ALLOC(StartGameContext);
	res_group_init(&ctx->rg);

	if(info == NULL) {
		global.is_practice_mode = false;
		ctx->current_stage = ctx->restart_stage = stageinfo_get_by_id(1);
	} else {
		global.is_practice_mode = (info->type != STAGE_EXTRA);
		ctx->current_stage = info;
		ctx->restart_stage = info;
	}

	Difficulty stagediff = info ? info->difficulty : D_Any;

	CallChain cc_pick_character = CALLCHAIN(start_game_do_pick_character, ctx);

	if(stagediff == D_Any) {
		if(difficulty_menu) {
			ctx->diff_menu = create_difficulty_menu(~0);
			enter_menu(ctx->diff_menu, cc_pick_character);
		} else {
			ctx->difficulty = progress.game_settings.difficulty;
			run_call_chain(&cc_pick_character, NULL);
		}
	} else {
		ctx->difficulty = stagediff;
		run_call_chain(&cc_pick_character, NULL);
	}
}

static void start_game_do_pick_character(CallChainResult ccr) {
	StartGameContext *ctx = ccr.ctx;
	MenuData *prev_menu = ccr.result;

	if(prev_menu) {
		if(prev_menu->state == MS_Dead) {
			start_game_do_cleanup(ccr);
			return;
		}

		// came here from the difficulty menu - update our setting
		ctx->difficulty = progress.game_settings.difficulty;
	}

	assert(ctx->char_menu == NULL);
	ctx->char_menu = create_char_menu();
	enter_menu(ctx->char_menu, CALLCHAIN(start_game_do_enter_stage, ctx));
}

static void reset_game(StartGameContext *ctx) {
	ctx->current_stage = ctx->restart_stage;

	global.gameover = GAMEOVER_NONE;
	replay_reset(&ctx->replay);
	replay_state_init_record(&global.replay.output, &ctx->replay);
	player_init(&global.plr);
	stats_init(&global.plr.stats);
	global.plr.mode = plrmode_find(
		progress.game_settings.character,
		progress.game_settings.shotmode
	);
	global.diff = ctx->difficulty;

	assert(global.plr.mode != NULL);
}

static void kill_aux_menus(StartGameContext *ctx) {
	kill_menu(ctx->char_menu);
	kill_menu(ctx->diff_menu);
	ctx->char_menu = ctx->diff_menu = NULL;
}

static void enter_stage_now(StartGameContext *ctx) {
	res_group_release(&ctx->rg);
	stage_enter(ctx->current_stage, &ctx->rg, CALLCHAIN(start_game_do_leave_stage, ctx));
}

static void start_game_do_enter_stage(CallChainResult ccr) {
	StartGameContext *ctx = ccr.ctx;
	MenuData *prev_menu = ccr.result;

	if(prev_menu && prev_menu->state == MS_Dead) {
		assert(prev_menu == ctx->char_menu);
		ctx->char_menu = NULL;

		if(!ctx->diff_menu) {
			start_game_do_cleanup(ccr);
		}

		return;
	}

	taisei_commit_persistent_data();
	kill_aux_menus(ctx);
	reset_game(ctx);
	enter_stage_now(ctx);
}

static void start_game_do_leave_stage(CallChainResult ccr) {
	StartGameContext *ctx = ccr.ctx;

	if(global.gameover == GAMEOVER_RESTART) {
		reset_game(ctx);
		enter_stage_now(ctx);
		return;
	}

	if(ctx->current_stage->type == STAGE_STORY && !global.is_practice_mode) {
		++ctx->current_stage;

		if(ctx->current_stage->type == STAGE_STORY) {
			enter_stage_now(ctx);
		} else {
			CallChain cc;

			if(global.gameover == GAMEOVER_WIN) {
				res_group_release(&ctx->rg);
				res_purge();
				credits_preload(&ctx->rg);
				cc = CALLCHAIN(start_game_do_show_ending, ctx);
			} else {
				cc = CALLCHAIN(start_game_do_cleanup, ctx);
			}

			ask_save_replay(&ctx->replay, cc);
		}
	} else {
		ask_save_replay(&ctx->replay, CALLCHAIN(start_game_do_cleanup, ctx));
	}
}

static void start_game_do_show_ending(CallChainResult ccr) {
	Player *plr = &global.plr;
	PlayerCharacter *character = plr->mode->character;
	PlrCharEndingCutscene *ec_id;

	if (plr->stats.total.continues_used) {
		ec_id = &character->ending.bad;
	} else {
		ec_id = &character->ending.good;
	}

	progress_track_ending(ec_id->ending_id);
	cutscene_enter(CALLCHAIN(start_game_do_show_credits, ccr.ctx), ec_id->cutscene_id);
}

static void start_game_do_show_credits(CallChainResult ccr) {
	credits_enter(CALLCHAIN(start_game_do_cleanup, ccr.ctx));
}

static void start_game_do_cleanup(CallChainResult ccr) {
	StartGameContext *ctx = ccr.ctx;
	replay_reset(&ctx->replay);
	kill_aux_menus(ctx);
	res_group_release(&ctx->rg);
	mem_free(ctx);
	global.gameover = GAMEOVER_NONE;
	replay_state_deinit(&global.replay.output);
	main_menu_update_practice_menus();
	audio_bgm_play(res_bgm("menu"), true, 0, 0);
}

void start_game(MenuData *m, void *arg) {
	start_game_internal(m, (StageInfo*)arg, true);
}

void start_game_no_difficulty_menu(MenuData *m, void *arg) {
	start_game_internal(m, (StageInfo*)arg, false);
}

void draw_menu_selector(float x, float y, float w, float h, float t) {
	Sprite *bg = res_sprite("part/smoke");
	r_mat_mv_push();
	r_mat_mv_translate(x, y, 0);
	r_mat_mv_scale(w / bg->w, h / bg->h, 1);
	r_mat_mv_rotate(t * 2 * DEG2RAD, 0, 0, 1);
	r_draw_sprite(&(SpriteParams) {
		.sprite = bg,
		.color = RGBA(0, 0, 0, 0.5 * (1 - transition.fade)),
		.shader = res_shader("sprite_default"),
	});
	r_mat_mv_pop();
}

void draw_menu_title(MenuData *m, const char *title) {
	Font *font = res_font("heading");
	float w = text_width(font, title, 0);
	float fade = menu_fade(m);
	float shown = 1 - fade;
	float x = 24 - (w + 48) * fade;  // 기존처럼 왼쪽에서 밀려 들어온다

	text_draw(title, &(TextParams) {
		.pos = { x, 44 },
		.font_ptr = font,
		.shader_ptr = res_shader("text_ui"),
		.color = RGBA_MUL_ALPHA(1, 1, 1, 0.9 * shown),
	});

	// 제목 아래 짧은 강조선
	draw_ui_panel(&(UIPanelParams) {
		.x = x + 20, .y = 56,
		.w = 40, .h = 3,
		.radius = 1.5,
		.fill = RGBA(0.96, 0.78, 0.42, 0.9 * shown),
	});
}

void draw_ui_panel(const UIPanelParams *p) {
	Color fill2 = p->fill2;

	if(fill2.r == 0 && fill2.g == 0 && fill2.b == 0 && fill2.a == 0) {
		fill2 = p->fill;
	}

	float gx = p->gradient_dir_x;
	float gy = p->gradient_dir_y;

	if(gx == 0 && gy == 0) {
		gy = 1;
	}

	r_state_push();
	r_shader("ui_panel");
	r_uniform_vec2("size", p->w, p->h);
	r_uniform_float("radius", p->radius);
	r_uniform_float("border", p->border);
	r_uniform_vec4_rgba("fill_color", p->fill);
	r_uniform_vec4_rgba("fill_color2", fill2);
	r_uniform_vec4_rgba("border_color", p->border_color);
	r_uniform_vec2("gradient_dir", gx, gy);
	r_color4(1, 1, 1, 1);
	r_mat_mv_push();
	r_mat_mv_translate(p->x, p->y, 0);
	r_mat_mv_scale(p->w, p->h, 1);
	r_draw_quad();
	r_mat_mv_pop();
	r_state_pop();
}

void draw_ui_dot(float x, float y, float diameter, Color color) {
	draw_ui_panel(&(UIPanelParams) {
		.x = x, .y = y,
		.w = diameter, .h = diameter,
		.radius = diameter,
		.fill = color,
	});
}

void draw_menu_list(MenuData *m, float x, float y, void (*draw)(MenuEntry*, int, int, void*), float scroll_threshold, void *userdata) {
	r_mat_mv_push();
	float offset = smoothmin(0, scroll_threshold * 0.8 - y - m->drawdata[2], 80);
	r_mat_mv_translate(x, y + offset, 0);

	draw_menu_selector(m->drawdata[0], m->drawdata[2], m->drawdata[1], 34, m->frames);
	ShaderProgram *text_shader = res_shader("text_default");

	dynarray_foreach(&m->entries, int i, MenuEntry *e, {
		float p = offset + 20*i;

		if(p < -y-10 || p > SCREEN_H+10)
			continue;

		float a = e->drawdata * 0.1;
		float o = (p < 0? 1-p/(-y-10) : 1);

		Color clr;

		if(e->action == NULL) {
			clr = RGBA_MUL_ALPHA(0.5, 0.5, 0.5, 0.5*o);
		} else {
			float ia = 1-a;
			clr = RGBA_MUL_ALPHA(0.9 + ia * 0.1, 0.6 + ia * 0.4, 0.2 + ia * 0.8, (0.7 + 0.3 * a)*o);
		}

		r_color(clr);

		if(draw && i < m->entries.num_elements-1) {
			draw(e, i, m->entries.num_elements, userdata);
		} else if(e->name) {
			text_draw(_(e->name), &(TextParams) {
				.pos = { 20 - e->drawdata, 20*i },
				.shader_ptr = text_shader,
				.color = clr,
			});
		}
	});

	r_mat_mv_pop();
}

void animate_menu_list_entry(MenuData *m, int i) {
	MenuEntry *e = dynarray_get_ptr(&m->entries, i);
	fapproach_asymptotic_p(&e->drawdata, 10 * (i == m->cursor), 0.2, 1e-4);
}

void animate_menu_list_entries(MenuData *m) {
	dynarray_foreach_idx(&m->entries, int i, {
		animate_menu_list_entry(m, i);
	});
}

void animate_menu_list(MenuData *m) {
	MenuEntry *s = dynarray_get_ptr(&m->entries, m->cursor);
	int w = text_width(res_font("standard"), s->name, 0);

	fapproach_asymptotic_p(&m->drawdata[0], 10 + w * 0.5, 0.1, 1e-5);
	fapproach_asymptotic_p(&m->drawdata[1], w * 2, 0.1, 1e-5);
	fapproach_asymptotic_p(&m->drawdata[2], 20 * m->cursor, 0.1, 1e-5);

	animate_menu_list_entries(m);
}

void menu_action_close(MenuData *menu, void *arg) {
	menu->state = MS_Dead;
}
