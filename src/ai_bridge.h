/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * AI bridge: optional, off-by-default observation + input hooks used by the
 * mini-danmaku-ai project (contract: docs/ai/CONTRACT.md in the main repo).
 * Nothing here changes game rules; it only reads state and replaces the
 * source of player input when the host explicitly asks for it.
 */

#pragma once
#include "taisei.h"

#include "player.h"
#include "replay/state.h"

typedef enum AIOwner {
	AI_OWNER_MANUAL,
	AI_OWNER_ACTIVE,
	AI_OWNER_SUSPENDED,
} AIOwner;

// Keep in sync with REASONS in ai/bridge/taisei-ai-bridge.mjs
typedef enum AIReason {
	AI_REASON_NONE,
	AI_REASON_PAUSE,
	AI_REASON_FOCUS_LOST,
	AI_REASON_DIALOG,
	AI_REASON_STAGE_END,
	AI_REASON_RESTART,
	AI_REASON_HOST_REQUEST,
	AI_REASON_USER_INPUT,
	AI_REASON_POLICY_ERROR,
	AI_REASON_POLICY_DEADLINE,
	AI_REASON_OBSERVATION_INVALID,
	AI_REASON_BRIDGE_ERROR,
	AI_REASON_GAME_OVER,
	AI_REASON_NOT_PLAYING,
	AI_REASON_COUNT,
} AIReason;

// --- hooks called from the game loop (all cheap when the bridge is idle) ---

void ai_bridge_capture(void);                       // end of a logic frame, before global.frames++
void ai_bridge_begin_input(void);                   // start of stage_input(), after events were polled
bool ai_bridge_owns_input(void);                    // true while owner == ai_active
uint ai_bridge_hold_flags(void);                    // inputflags the AI wants this tick
void ai_bridge_apply_edges(Player *plr, ReplayState *rpy_out);
bool ai_bridge_filter_key(int key, bool is_down);   // true = swallow this gameplay key event

void ai_bridge_on_stage_enter(uint64_t seed, int seed_source);
void ai_bridge_on_stage_leave(void);
void ai_bridge_on_stage_finish(int gameover);
void ai_bridge_on_menu_enter(AIReason reason);
void ai_bridge_on_menu_leave(void);
bool ai_bridge_seed_override(uint64_t *seed);

void ai_bridge_on_hit(void);
void ai_bridge_on_death(bool counted);
void ai_bridge_on_bomb(void);
void ai_bridge_on_cheat(void);
