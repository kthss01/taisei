/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#pragma once
#include "taisei.h"

#include "color.h"
#include "menu.h"

// 메뉴 공통 색: 어두운 남색 바탕 위 흰 글자, 선택 강조는 따뜻한 금색
#define UI_COLOR_ACCENT RGBA(0.96, 0.78, 0.42, 1)

// 둥근 사각형 패널(shader/ui_panel). 좌표는 중심, 크기는 px.
// 색은 알파를 미리 곱하지 않은 값으로 준다(셰이더가 곱한다).
typedef struct UIPanelParams {
	float x, y;
	float w, h;
	float radius;          // 모서리 반경(px). 크기의 절반 이상이면 원이 된다
	float border;          // 테두리 두께(px). 0이면 테두리 없음
	Color fill;            // 채움색(그라데이션 시작)
	Color fill2;           // 그라데이션 끝 색. 모두 0이면 fill과 같게 취급한다
	Color border_color;    // 테두리색
	float gradient_dir_x;  // 그라데이션 방향. 둘 다 0이면 위→아래
	float gradient_dir_y;
} UIPanelParams;

void start_game(MenuData *m, void *arg);
void start_game_no_difficulty_menu(MenuData *m, void *arg);
void draw_menu_selector(float x, float y, float w, float h, float t);
void draw_menu_title(MenuData *m, const char *title);
void draw_menu_list(MenuData *m, float x, float y, void (*draw)(MenuEntry*, int, int, void*), float scroll_threshold, void *userdata);
void draw_ui_panel(const UIPanelParams *p);
void draw_ui_dot(float x, float y, float diameter, Color color);
void animate_menu_list(MenuData *m);
void animate_menu_list_entries(MenuData *m);
void animate_menu_list_entry(MenuData *m, int i);
void menu_action_close(MenuData *menu, void *arg);
