/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2026, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2026, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "wriggle.h"

#include "boss.h"
#include "i18n/i18n.h"

Boss *stage2_spawn_wriggle(cmplx pos) {
	Boss *wriggle = create_boss(N_("RANDONNEUR"), "wriggle", pos);
	wriggle->glowcolor = RGB(0.25, 0.3, 0.6);
	wriggle->shadowcolor = RGBA_MUL_ALPHA(0.9, 0.75, 0.35, 0.5);
	return wriggle;
}
