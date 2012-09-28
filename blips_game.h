/* blips_game.h */
/* Started 9/14/12 by Tevis Tsai. */

#ifndef _BLIPS_GAME_H
#define _BLIPS_GAME_H 1

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

typedef struct blips_game
{
	/* world tiles keyed to their paths */
	string_map *world_tile_map;

//	ai **ais;
	int num_ais;
	breakable **breakables;
	int num_breakables;
	collectible **collectibles;
	int num_collectibles;
	creature **creatures;
	int num_creatures;
	projectile **projectiles;
	int num_projectiles;

	creature **players;  /* size of this array is always determined by campaign->num_players */
	creature_type **player_types;  /* always as large as the previous */

	/* keyed types not appearing as world tile symbols (keyed to their path strings) */
	string_map *ai_types_map;

	string_map *pr_types_map;

	/* keyed types (i.e. types that appear as symbols in world tile ascii maps) */
	string_map *br_types_map;
	string_map *co_types_map;
	string_map *cr_types_map;

	world_tile *active_world_tile;
	int active_world_tile_cleared;  /* has/have the player(s) cleared this tile yet? */

	blips_campaign *campaign;
} blips_game;

blips_game* blips_game_create(void);
void blips_game_destroy(blips_game *bgame);

/* externally called requests */

void blips_game_load_campaign(blips_game *bgame,char *path);

void blips_game_step(blips_game *bgame,blips_input_state **inputs);

world_tile* blips_game_active_world_tile(blips_game *bgame);

/* private functions */

void blips_game_load_world_tiles(blips_game *bgame);

int blips_game_add_world_tile(blips_game *bgame,char *path);

	/* clear objects from game */
void blips_game_despawn(blips_game *bgame);
	/* spawn objects which follow spawn_trigger trigger from active tile */
void blips_game_spawn(blips_game *bgame,spawn_trigger trigger);
	/* what direction, if any, are players trying to go? */
int blips_game_players_leaving_world_tile(blips_game *bgame);
	/* change to a new tile */
void blips_game_change_active_world_tile(blips_game *bgame,int direction);

	/* load object key specified by campaign */
void blips_game_load_object_types(blips_game *bgame);

	/* load player creatures */
void blips_game_load_players(blips_game *bgame);

	/* remove projectile of this index */
void blips_game_remove_projectile_by_index(blips_game *bgame,int i);

	/* apply ai_type directives to given creature */
void blips_game_apply_ai_type_to_creature(blips_game *bgame,ai_type *ai_type_ptr,creature *cr);

	/* change object location based on speed, orientation */
int blips_game_move_creature(blips_game *bgame,creature *cr);
void blips_game_move_projectile(blips_game *bgame,projectile *pr);

	/* spawn the appropriate projectile from creature */
void blips_game_spawn_projectile_from_creature(blips_game *bgame,creature *cr);

	/* remove projectiles which have left the tile */
void blips_game_remove_projectiles_outside_boundaries(blips_game *bgame);

	/* keep creatures from bumping into things */
int blips_game_creature_intersects_creatures(blips_game *bgame,creature *cr);
int blips_game_creature_intersects_boundaries(blips_game *bgame,creature *cr);
int blips_game_creature_intersects_barriers(blips_game *bgame,creature *cr);
int blips_game_creature_intersects_breakables(blips_game *bgame,creature *cr);

	/* when projectiles impact things */
int blips_game_check_projectile_for_impact(blips_game *bgame,projectile *pr);
int blips_game_projectile_intersects_barriers(blips_game *bgame,projectile *pr);

	/* AI directives */
creature* ai_directives_get_nearest_enemy_creature(blips_game *bgame,creature *cr);
projectile* ai_directives_get_nearest_enemy_projectile(blips_game *bgame,creature *cr);

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

