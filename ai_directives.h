/* ai_directives.h */
/* Started 9/27/12 by Tevis Tsai. */

#ifndef _AI_DIRECTIVES_H
#define _AI_DIRECTIVES_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _TIME_H
#include<time.h>
#endif

#ifndef _MATH_H
#include<math.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _STRING_MAP_H
#include"string_map.h"
#endif

#ifndef _AI_TYPE_H
#include"ai_type.h"
#endif

#ifndef _BREAKABLE_TYPE_H
#include"breakable_type.h"
#endif

#ifndef _COLLECTIBLE_TYPE_H
#include"collectible_type.h"
#endif

#ifndef _CREATURE_TYPE_H
#include"creature_type.h"
#endif

#ifndef _PROJECTILE_TYPE_H
#include"projectile_type.h"
#endif

#ifndef _AI_H
#include"ai.h"
#endif

#ifndef _BREAKABLE_H
#include"breakable.h"
#endif

#ifndef _COLLECTIBLE_H
#include"collectible.h"
#endif

#ifndef _CREATURE_H
#include"creature.h"
#endif

#ifndef _PROJECTILE_H
#include"projectile.h"
#endif

#ifndef _WORLD_TILE_H
#include"world_tile.h"
#endif

#ifndef _BLIPS_INPUT_STATE_H
#include"blips_input_state.h"
#endif

#ifndef _BLIPS_CAMPAIGN_H
#include"blips_campaign.h"
#endif

#ifndef _BLIPS_GAME_H
#include"blips_game.h"
#endif

creature* ai_directives_get_nearest_creature(blips_game *bgame,creature *cr);
projectile* ai_directives_get_nearest_projectile(blips_game *bgame,creature *cr);

void ai_directives_move_dodge(blips_game *bgame,creature *cr,projectile *enemy_pr);
void ai_directives_move_flee(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_move_close(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_move_wander(blips_game *bgame,creature *cr);
void ai_directives_move_patrol_ns(blips_game *bgame,creature *cr);
void ai_directives_move_patrol_ew(blips_game *bgame,creature *cr);

void ai_directives_aim_face(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_aim_spin(blips_game *bgame,creature *cr);
void ai_directives_aim_lag_face(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_aim_lead_face(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_aim_as_velocity(blips_game *bgame,creature *cr);

void ai_directives_fire_spam(blips_game *bgame,creature *cr);
void ai_directives_fire_sight_first(blips_game *bgame,creature *cr,creature *enemy_cr);
void ai_directives_fire_spurt(blips_game *bgame,creature *cr,creature *enemy_cr);

#endif

