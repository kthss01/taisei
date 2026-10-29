/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "difficultyselect.h"

#include "common.h"
#include "difficulty.h"
#include "i18n/i18n.h"
#include "mainmenu.h"
#include "progress.h"
#include "resource/font.h"
#include "video.h"

// FIXME: put this into the menu struct somehow (drawdata is a bad system)
static Color diff_color;

static void set_difficulty(MenuData *m, void *d) {
	progress.game_settings.difficulty = (Difficulty)(uintptr_t)d;
}

static void update_difficulty_menu(MenuData *menu) {
	menu->drawdata[0] += (menu->cursor-menu->drawdata[0])*0.1;

	dynarray_foreach(&menu->entries, int i, MenuEntry *e, {
		e->drawdata += 0.2 * ((i == menu->cursor) - e->drawdata);
	});

	diff_color = color_approach(diff_color, difficulty_color(menu->cursor + D_Easy), 0.1);
}

MenuData* create_difficulty_menu(uint32_t mask) {
	MenuData *m = alloc_menu();

	m->draw = draw_difficulty_menu;
	m->logic = update_difficulty_menu;
	m->transition = TransFadeBlack;
	m->flags = MF_Abortable;

	add_menu_entry(m, _("For those just beginning\ntheir academic careers"),
		 (mask & (1<<D_Easy)) ? set_difficulty : NULL, (void *)D_Easy);
	add_menu_entry(m, _("All those years of study\nhave finally paid off"),
		 (mask & (1<<D_Normal)) ? set_difficulty : NULL, (void *)D_Normal);
	add_menu_entry(m, _("You must really\nlove books"),
		 (mask & (1<<D_Hard)) ? set_difficulty : NULL, (void *)D_Hard);
	add_menu_entry(m, _("You either die a student,\nor live long enough to\nbecome a professor"),
		 (mask & (1<<D_Lunatic)) ? set_difficulty : NULL, (void *)D_Lunatic);

	dynarray_foreach(&m->entries, int i, MenuEntry *e, {
		if(e->action) {
			m->cursor = i;
			break;
		}
	});

	dynarray_foreach(&m->entries, int i, MenuEntry *e, {
		Difficulty d = (Difficulty)(uintptr_t)e->arg;
		if(d == progress.game_settings.difficulty && e->action) {
			m->cursor = i;
			break;
		}
	});

	return m;
}

void draw_difficulty_menu(MenuData *menu) {
	r_state_push();

	draw_main_menu_bg(menu, 0, 0, 0.05, "menu/mainmenubg", "stage1/cirnobg");
	draw_menu_title(menu, _("Select Difficulty"));

	float shown = 1 - menu_fade(menu);
	Color dc = diff_color;

	const float panel_w = 620;
	const float panel_h = 190;
	const float panel_x = SCREEN_W / 2;
	const float panel_y = SCREEN_H / 2 + 10;

	// 중앙 유리 패널: 테두리는 현재 난이도 색
	draw_ui_panel(&(UIPanelParams) {
		.x = panel_x, .y = panel_y,
		.w = panel_w, .h = panel_h,
		.radius = 18, .border = 1,
		.fill = RGBA(0.03, 0.04, 0.08, 0.74 * shown),
		.fill2 = RGBA(0.03, 0.04, 0.08, 0.58 * shown),
		.border_color = RGBA(dc.r, dc.g, dc.b, 0.85 * shown),
	});

	// 왼쪽: 붓글씨 난이도 그림을 세로로 넘기는 캐러셀
	const float carousel_x = panel_x - panel_w / 2 + 150;
	ShaderProgram *sprite_shader = res_shader("sprite_default");

	dynarray_foreach(&menu->entries, int i, MenuEntry *e, {
		float sel = e->drawdata;
		float scale = (0.32 + 0.26 * sel) * (e->action ? 1.0f : 0.85f);
		float yofs = 170 * tanh(0.8 * (i - menu->drawdata[0]));
		float a = (0.25 + 0.75 * sel) * shown;

		r_draw_sprite(&(SpriteParams) {
			.sprite = res_sprite(difficulty_sprite_name(D_Easy + i)),
			.shader = sprite_shader,
			.pos = { carousel_x, panel_y + yofs },
			.color = RGBA(a, a, a, a),
			.scale.both = scale,
		});
	});

	// 오른쪽: 난이도 이름과 설명. 높은 난이도일수록 살짝 떨린다(기존 연출, 진폭은 절반)
	Difficulty diff = D_Easy + menu->cursor;
	float amp = menu->cursor / 4.0;
	float shake = 0.3 * amp * menu->frames;
	const float text_x = panel_x - panel_w / 2 + 300;
	ShaderProgram *text_shader = res_shader("text_ui");

	text_draw(difficulty_name(diff), &(TextParams) {
		.pos = { text_x + amp * sin(shake), panel_y - 28 + amp * cos(1.57 * shake) },
		.font = "title",
		.shader_ptr = text_shader,
		.color = RGBA_MUL_ALPHA(dc.r, dc.g, dc.b, shown),
	});

	text_draw_wrapped(dynarray_get(&menu->entries, menu->cursor).name, 280, &(TextParams) {
		.pos = { text_x, panel_y + 6 },
		.font = "standard",
		.shader_ptr = text_shader,
		.color = RGBA_MUL_ALPHA(1, 1, 1, 0.85 * shown),
	});

	// 아래: 난이도 수만큼 점 인디케이터
	int n = menu->entries.num_elements;

	dynarray_foreach(&menu->entries, int i, MenuEntry *e, {
		float sel = e->drawdata;
		Color c = difficulty_color(D_Easy + i);
		float a = (0.3 + 0.7 * sel) * shown * (e->action ? 1.0f : 0.4f);
		float x = panel_x + (i - (n - 1) * 0.5f) * 24;
		draw_ui_dot(x, panel_y + panel_h / 2 + 26, 8 + 3 * sel, RGBA(c.r, c.g, c.b, a));
	});

	r_state_pop();
}
