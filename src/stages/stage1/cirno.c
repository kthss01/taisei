/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "cirno.h"

#include "common_tasks.h"
#include "i18n/i18n.h"

void stage1_draw_cirno_spellbg(Boss *c, int time) {
	// 자전거 테마 1차: 스펠 배경을 MINIVELO 콘셉트 시트에서 만든 임시 타일로 바꾼다(메뉴 배경 cirnobg는 유지).
	r_color4(0.5, 0.5, 0.5, 1.0);
	fill_viewport(time/700.0, time/700.0, 1, "stage1/minivelobg");
	r_blend(BLEND_MOD);
	r_color4(0.7, 0.7, 0.7, 0.5);
	fill_viewport(-time/700.0 + 0.5, time/700.0+0.5, 0.4, "stage1/minivelobg");
	r_blend(BLEND_PREMUL_ALPHA);
	r_color4(0.35, 0.35, 0.35, 0.0);
	fill_viewport(0, -time/100.0, 0, "stage1/snowlayer");
	r_color4(1.0, 1.0, 1.0, 1.0);
}

Boss *stage1_spawn_cirno(cmplx pos) {
	// 자전거 의인화 테마 1차 시험: 표시 이름과 색만 MINIVELO(임시 이름, 민트·크림)로 바꾸고
	// 내부 id "cirno"와 패턴·판정은 그대로 둔다.
	Boss *cirno = create_boss(N_("MINIVELO"), "cirno", pos);
	boss_set_portrait(cirno, "cirno", NULL, "normal");
	cirno->shadowcolor = RGBA_MUL_ALPHA(0.70, 0.95, 0.85, 0.25);
	cirno->glowcolor = RGB(0.30, 0.70, 0.58);
	return cirno;
}

void stage1_cirno_wander(Boss *boss, real dist, real lower_bound) {
	Rect bounds = viewport_bounds(64);
	bounds.bottom = lower_bound;
	boss->move.attraction_point = common_wander(boss->pos, dist, bounds);
}
