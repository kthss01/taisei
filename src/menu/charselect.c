/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "charselect.h"

#include "audio/audio.h"
#include "common.h"
#include "events.h"
#include "i18n/i18n.h"
#include "memory/scratch.h"
#include "menu.h"
#include "menu/mainmenu.h"
#include "plrmodes.h"
#include "portrait.h"
#include "progress.h"
#include "util/glm.h"
#include "util/graphics.h"
#include "util/strbuf.h"
#include "video.h"

#define SELECTED_SUBSHOT(m) (((CharMenuContext*)(m)->context)->subshot)
#define DESCRIPTION_WIDTH 308

enum {
	F_HAPPY,
	F_NORMAL,
	F_PUZZLED,
	F_SMUG,
	F_SURPRISED,
	F_UNAMUSED,
	NUM_FACES,
};

#define FACENAME_LEN 32
static const char facedefs[NUM_CHARACTERS][NUM_FACES][FACENAME_LEN] = {
	[PLR_CHAR_REIMU]  = {
		[F_HAPPY]     = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, happy),
		[F_NORMAL]    = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, normal),
		[F_PUZZLED]   = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, puzzled),
		[F_SMUG]      = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, smug),
		[F_SURPRISED] = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, surprised),
		[F_UNAMUSED]  = PORTRAIT_STATIC_FACE_SPRITE_NAME(reimu, unamused),
	},
	[PLR_CHAR_MARISA]  = {
		[F_HAPPY]     = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, happy),
		[F_NORMAL]    = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, normal),
		[F_PUZZLED]   = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, puzzled),
		[F_SMUG]      = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, smug),
		[F_SURPRISED] = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, surprised),
		[F_UNAMUSED]  = PORTRAIT_STATIC_FACE_SPRITE_NAME(marisa, unamused),
	},
	[PLR_CHAR_YOUMU]  = {
		[F_HAPPY]     = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, happy),
		[F_NORMAL]    = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, normal),
		[F_PUZZLED]   = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, puzzled),
		[F_SMUG]      = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, smug),
		[F_SURPRISED] = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, surprised),
		[F_UNAMUSED]  = PORTRAIT_STATIC_FACE_SPRITE_NAME(youmu, unamused),
	},
};

// 캐릭터별 포인트 색: 초상화 림라이트·글로우·칭호·칩에 쓴다
static const Color char_accent_colors[NUM_CHARACTERS] = {
	[PLR_CHAR_REIMU]  = { { 0.93, 0.36, 0.38, 1 } },
	[PLR_CHAR_MARISA] = { { 0.97, 0.80, 0.40, 1 } },
	[PLR_CHAR_YOUMU]  = { { 0.50, 0.86, 0.74, 1 } },
};

typedef struct CharMenuContext {
	int8_t subshot;
	int8_t char_draw_order[NUM_CHARACTERS];
	int8_t prev_selected_char;
} CharMenuContext;

static void set_player_mode(MenuData *m, void *p) {
	progress.game_settings.character = (CharacterID)(uintptr_t)p;
	progress.game_settings.shotmode = SELECTED_SUBSHOT(m);
}

static void char_menu_input(MenuData*);

static void update_char_draw_order(MenuData *menu) {
	CharMenuContext *ctx = menu->context;

	for(int i = 0; i < NUM_CHARACTERS; ++i) {
		if(ctx->char_draw_order[i] == menu->cursor) {
			while(i) {
				ctx->char_draw_order[i] = ctx->char_draw_order[i - 1];
				ctx->char_draw_order[i - 1] = menu->cursor;
				--i;
			}

			break;
		}
	}
}

static void update_char_menu(MenuData *menu) {
	dynarray_foreach(&menu->entries, int i, MenuEntry *e, {
		e->drawdata += 0.05 * ((menu->cursor != i) - e->drawdata);
	});

	MenuEntry *cursor_entry = dynarray_get_ptr(&menu->entries, menu->cursor);

	PlayerCharacter *pchar = plrchar_get((CharacterID)(uintptr_t)cursor_entry->arg);
	assume(pchar != NULL);

	PlayerMode *m = plrmode_find(pchar->id, SELECTED_SUBSHOT(menu));
	assume(m != NULL);

	Font *font = res_font("standard");

	StringBuffer buf = { acquire_scratch_arena() };
	text_wrap(font, _(m->description), DESCRIPTION_WIDTH, &buf);
	double height = text_height(font, buf.start, 0) + font_get_lineskip(font) * 2;
	release_scratch_arena(buf.arena);

	fapproach_asymptotic_p(&menu->drawdata[0], SELECTED_SUBSHOT(menu) - PLR_SHOT_A, 0.1, 1e-5);
	fapproach_asymptotic_p(&menu->drawdata[1], 1 - cursor_entry->drawdata, 0.1, 1e-5);
	fapproach_asymptotic_p(&menu->drawdata[2], height, 0.1, 1e-5);
}

static void end_char_menu(MenuData *m) {
	mem_free(m->context);
}

static void transition_to_game(double fade) {
	fade_out(pow(max(0, (fade - 0.5) * 2), 2));
}

MenuData* create_char_menu(void) {
	MenuData *m = alloc_menu();

	m->input = char_menu_input;
	m->draw = draw_char_menu;
	m->logic = update_char_menu;
	m->end = end_char_menu;
	m->transition = TransFadeBlack;
	m->flags = MF_Abortable;

	auto ctx = ALLOC(CharMenuContext, {
		.subshot = progress.game_settings.shotmode,
		.prev_selected_char = -1,
	});
	m->context = ctx;

	for(uintptr_t i = 0; i < NUM_CHARACTERS; ++i) {
		MenuEntry *e = add_menu_entry(m, NULL, set_player_mode, (void*)i);
		e->transition = transition_to_game;
		e->drawdata = 1;

		if(i == progress.game_settings.character) {
			m->cursor = i;
		}
	}

	for(CharacterID c = 0; c < NUM_CHARACTERS; ++c) {
		ctx->char_draw_order[c] = c;
	}

	m->drawdata[1] = 1;

	return m;
}

void draw_char_menu(MenuData *menu) {
	CharMenuContext *ctx = menu->context;

	r_state_push();

	const char *const prefixes[] = {
		_("Intuition"),
		_("Science"),
	};

	assert(menu->cursor < 3);
	PlayerCharacter *selected_char = plrchar_get((CharacterID)(uintptr_t)dynarray_get(&menu->entries, menu->cursor).arg);
	Color accent = char_accent_colors[selected_char->id];
	float shown = 1 - menu_fade(menu);
	float settled = menu->drawdata[1];  // 선택 캐릭터가 자리를 잡은 정도(0~1)

	draw_main_menu_bg(menu, SCREEN_W/4+100, 0, 0.1 * (0.5 + 0.5 * settled), "menu/mainmenubg", selected_char->menu_texture_name);

	// 왼쪽 열 가독성용 그라데이션
	draw_ui_panel(&(UIPanelParams) {
		.x = 230, .y = SCREEN_H / 2,
		.w = 460, .h = SCREEN_H,
		.fill = RGBA(0.02, 0.03, 0.06, 0.66 * shown),
		.fill2 = RGBA(0.02, 0.03, 0.06, 0),
		.gradient_dir_x = 1,
	});

	draw_menu_title(menu, _("Select Character"));

	Font *font_title = res_font("title");
	Font *font_heading = res_font("heading");
	Font *font_std = res_font("standard");
	Font *font_small = res_font("small");
	ShaderProgram *text_shader = res_shader("text_ui");
	ShaderProgram *portrait_shader = res_shader("sprite_portrait");
	ShaderProgram *sprite_shader = res_shader("sprite_default");

	const float col_x = 48;                        // 왼쪽 열 시작
	const float portrait_cx = SCREEN_W / 2 + 230;  // 초상화 중심 x

	// 초상화 뒤 글로우: 선택 캐릭터 색으로 천천히 숨 쉰다
	{
		float breath = 0.5 + 0.5 * sin(menu->frames / 40.0);
		float g = (0.5 + 0.12 * breath) * shown * settled;

		r_draw_sprite(&(SpriteParams) {
			.sprite = res_sprite("ui/glow"),
			.shader = sprite_shader,
			.pos = { portrait_cx, SCREEN_H * 0.55 },
			.scale.both = 3.2,
			.color = RGBA(accent.r * g, accent.g * g, accent.b * g, 0),
		});
	}

	CharacterID current_char = 0;

	dynarray_foreach_idx(&menu->entries, int j, {
		CharacterID i = ctx->char_draw_order[j];
		MenuEntry *e = dynarray_get_ptr(&menu->entries, i);

		PlayerCharacter *pchar = plrchar_get((CharacterID)(uintptr_t)e->arg);
		assert(pchar != NULL);
		assert(pchar->id == i);

		Sprite *spr = portrait_get_base_sprite(pchar->lower_name, NULL);  // TODO cache this
		Color accent_i = char_accent_colors[i];

		if(menu->cursor == i) {
			current_char = pchar->id;
		}

		float o = 1 - e->drawdata*2;
		const char *face;

		if(menu->selected == i) {
			face = facedefs[i][SELECTED_SUBSHOT(menu) == PLR_SHOT_A ? F_HAPPY : F_SMUG];
		} else if(fabs(o - 1) < 1e-1) {
			face = facedefs[i][F_NORMAL];
		} else if(menu->cursor == i) {
			face = facedefs[i][F_SURPRISED];
		} else {
			face = facedefs[i][F_UNAMUSED];
		}

		// 선택되지 않은 캐릭터는 오른쪽으로 미끄러지며 어두워진다
		float pofs = glm_ease_quad_in(clamp(e->drawdata * 1.5f - 0.5f, 0.0f, 1.0f));

		if(i != menu->selected) {
			pofs = lerp(pofs, 1, menu_fade(menu));
		}

		float pbrightness = 0.55 + 0.45 * (1 - e->drawdata);
		float breath_y = 2 * sin(menu->frames / 50.0);

		SpriteParams portrait_params = {
			.pos = { portrait_cx + 260 * pofs, SCREEN_H - spr->h * 0.5 + breath_y },
			.sprite = spr,
			.shader = portrait_shader,
			.color = RGBA_MUL_ALPHA(pbrightness, pbrightness, pbrightness, 1 - pofs),
			// 바닥 페이드 시작, 림라이트 세기, 채도, 대비
			.shader_params.vec = { 0.84, 0.55, 1.08, 1.06 },
			.shader_params.mat = {
				{ accent_i.r, accent_i.g, accent_i.b, 3.0 },  // 림 색, 두께(텍셀)
				{ 0.8, -0.6, 0, 0 },                           // 빛 방향: 오른쪽 위
			},
		};

		r_draw_sprite(&portrait_params);

		// 표정 파츠는 페이드·림 없이 그대로 얹는다
		portrait_params.sprite = res_sprite(face);
		portrait_params.shader_params.vec[0] = 2;
		portrait_params.shader_params.vec[1] = 0;
		r_draw_sprite(&portrait_params);

		// 이름과 칭호: 선택된 캐릭터만, 전환 때 왼쪽에서 밀려 들어온다
		float vis = clamp(o, 0.0f, 1.0f) * shown;

		if(vis > 1e-3) {
			float slide = 24 * (1 - vis);

			text_draw(_(pchar->full_name), &(TextParams) {
				.pos = { col_x - slide, 168 },
				.font_ptr = font_title,
				.shader_ptr = text_shader,
				.color = RGBA_MUL_ALPHA(1, 1, 1, vis),
			});

			text_draw(_(pchar->title), &(TextParams) {
				.pos = { col_x - slide, 196 },
				.font_ptr = font_std,
				.shader_ptr = text_shader,
				.color = color_mul_scalar(accent_i, vis),
			});
		}
	});

	// 샷 모드 카드: 활성 카드만 설명을 펼친다
	ShotModeID current_subshot = SELECTED_SUBSHOT(menu);
	float f = menu->drawdata[0] - PLR_SHOT_A;
	const float card_w = DESCRIPTION_WIDTH + 32;
	const float card_x = col_x + card_w / 2;
	const float left = col_x + 16;
	float card_top = 236;

	for(ShotModeID shot = PLR_SHOT_A; shot < NUM_SHOT_MODES_PER_CHARACTER; shot++) {
		PlayerMode *mode = plrmode_find(current_char, shot);
		assume(mode != NULL);
		int shotidx = shot - PLR_SHOT_A;

		float act = clamp(1 - fabs(f - shotidx), 0.0f, 1.0f);  // 1 = 활성 카드
		float h = 50 + act * menu->drawdata[2];

		draw_ui_panel(&(UIPanelParams) {
			.x = card_x, .y = card_top + h / 2,
			.w = card_w, .h = h,
			.radius = 12, .border = 1,
			.fill = RGBA(0.03, 0.04, 0.08, (0.35 + 0.4 * act) * shown),
			.fill2 = RGBA(0.03, 0.04, 0.08, (0.25 + 0.4 * act) * shown),
			.border_color = RGBA(accent.r, accent.g, accent.b, (0.2 + 0.7 * act) * shown),
		});

		text_draw(prefixes[shotidx], &(TextParams) {
			.pos = { left, card_top + 20 },
			.font_ptr = font_small,
			.shader_ptr = text_shader,
			.color = color_mul_scalar(accent, (0.55 + 0.45 * act) * shown),
		});

		text_draw(_(mode->name), &(TextParams) {
			.pos = { left, card_top + 42 },
			.font_ptr = font_heading,
			.shader_ptr = text_shader,
			.color = RGBA_MUL_ALPHA(1, 1, 1, (0.55 + 0.45 * act) * shown),
		});

		if(act > 1e-3 && shot == current_subshot) {
			text_draw_wrapped(_(mode->description), DESCRIPTION_WIDTH, &(TextParams) {
				.pos = { left, card_top + 68 },
				.font_ptr = font_std,
				.shader_ptr = text_shader,
				.color = RGBA_MUL_ALPHA(1, 1, 1, 0.85 * act * shown),
			});
		}

		card_top += h + 10;
	}

	// 아래: 캐릭터 칩 — 세 명을 한눈에 보여 주고, 선택된 칩은 그 캐릭터 색으로 채운다
	const float chip_y = 522;
	float pulse = (0.5 + 0.3 * sin(menu->frames / 20.0)) * shown * settled;
	float chip_x = col_x + 22;
	ShaderProgram *chip_text_shader = res_shader("text_default");

	text_draw("‹", &(TextParams) {
		.pos = { col_x, chip_y + 8 },
		.font_ptr = font_heading,
		.shader_ptr = text_shader,
		.color = color_mul_scalar(accent, pulse),
	});

	for(CharacterID c = 0; c < NUM_CHARACTERS; ++c) {
		PlayerCharacter *pc = plrchar_get(c);
		float sel = 1 - dynarray_get(&menu->entries, c).drawdata;
		const char *name = _(pc->proper_name);
		float tw = text_width(font_std, name, 0);
		float cw = tw + 28;
		Color ac = char_accent_colors[c];

		draw_ui_panel(&(UIPanelParams) {
			.x = chip_x + cw / 2, .y = chip_y,
			.w = cw, .h = 28,
			.radius = 14, .border = 1,
			.fill = RGBA(ac.r, ac.g, ac.b, 0.9 * sel * shown),
			.border_color = RGBA(1, 1, 1, (0.18 + 0.4 * sel) * shown),
		});

		Color text_color = color_lerp(RGBA_MUL_ALPHA(1, 1, 1, 0.7), RGBA(0.06, 0.06, 0.1, 1), sel);

		text_draw(name, &(TextParams) {
			.pos = { chip_x + 14, chip_y + 6 },
			.font_ptr = font_std,
			.shader_ptr = chip_text_shader,
			.color = color_mul_scalar(text_color, shown),
		});

		chip_x += cw + 8;
	}

	text_draw("›", &(TextParams) {
		.pos = { chip_x + 2, chip_y + 8 },
		.font_ptr = font_heading,
		.shader_ptr = text_shader,
		.color = color_mul_scalar(accent, pulse),
	});

	text_draw(_("‹ ›  Character     Up / Down  Shot     Z  Confirm     X  Back"), &(TextParams) {
		.pos = { col_x, SCREEN_H - 26 },
		.font_ptr = font_small,
		.shader_ptr = text_shader,
		.color = RGBA_MUL_ALPHA(1, 1, 1, 0.45 * shown),
	});

	r_state_pop();
}

static bool char_menu_input_handler(SDL_Event *event, void *arg) {
	MenuData *menu = arg;
	CharMenuContext *ctx = menu->context;
	TaiseiEvent type = TAISEI_EVENT(event->type);

	int prev_cursor = menu->cursor;

	if(type == TE_MENU_CURSOR_RIGHT) {
		play_sfx_ui("generic_shot");
		menu->cursor++;
	} else if(type == TE_MENU_CURSOR_LEFT) {
		play_sfx_ui("generic_shot");
		menu->cursor--;
	} else if(type == TE_MENU_CURSOR_DOWN) {
		play_sfx_ui("generic_shot");
		ctx->subshot++;
	} else if(type == TE_MENU_CURSOR_UP) {
		play_sfx_ui("generic_shot");
		ctx->subshot--;
	} else if(type == TE_MENU_ACCEPT) {
		play_sfx_ui("shot_special1");
		menu->selected = menu->cursor;
		menu->transition_in_time = FADE_TIME * 1.5;
		menu->transition_out_time = FADE_TIME * 3;
		close_menu(menu);
	} else if(type == TE_MENU_ABORT) {
		play_sfx_ui("hit");
		close_menu(menu);
	}

	menu->cursor = (menu->cursor % menu->entries.num_elements) + menu->entries.num_elements * (menu->cursor < 0);
	ctx->subshot = (ctx->subshot % NUM_SHOT_MODES_PER_CHARACTER) + NUM_SHOT_MODES_PER_CHARACTER * (ctx->subshot < 0);

	if(menu->cursor != prev_cursor) {
		if(ctx->prev_selected_char != menu->cursor || dynarray_get(&menu->entries, menu->cursor).drawdata > 0.95) {
			ctx->prev_selected_char = prev_cursor;
			update_char_draw_order(menu);
		}
	}

	return false;
}

static void char_menu_input(MenuData *menu) {
	events_poll((EventHandler[]){
		{ .proc = char_menu_input_handler, .arg = menu },
		{}
	}, EFLAG_MENU);
}

void preload_char_menu(ResourceGroup *rg) {
	for(int i = 0; i < NUM_CHARACTERS; ++i) {
		PlayerCharacter *pchar = plrchar_get(i);
		portrait_preload_base_sprite(rg, pchar->lower_name, NULL, RESF_DEFAULT);
		res_group_preload(rg, RES_TEXTURE, RESF_DEFAULT, pchar->menu_texture_name, NULL);
	}

	char *p = (char*)facedefs;

	for(int i = 0; i < sizeof(facedefs) / FACENAME_LEN; ++i) {
		res_group_preload(rg, RES_SPRITE, RESF_DEFAULT, p + i * FACENAME_LEN, NULL);
	}
	portrait_preload_face_sprite(rg, "reimu", "annoyed", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "reimu", "assertive", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "reimu", "irritated", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "reimu", "outraged", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "reimu", "sigh", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "reimu", "unsettled", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "marisa", "sweat_smile", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "marisa", "inquisitive", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "youmu", "eeeeh", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "youmu", "embarrassed", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "youmu", "eyes_closed", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "youmu", "relaxed", RESF_DEFAULT);
	portrait_preload_face_sprite(rg, "youmu", "sigh", RESF_DEFAULT);

	// 캐릭터 선택 화면 연출용 자원
	res_group_preload(rg, RES_SPRITE, RESF_DEFAULT, "ui/glow", NULL);
	res_group_preload(rg, RES_SHADER_PROGRAM, RESF_DEFAULT, "ui_panel", "sprite_portrait", "text_ui", NULL);
	res_group_preload(rg, RES_FONT, RESF_DEFAULT, "title", "heading", NULL);
}
