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
	world_tile *world_tiles;
	world_tile *active_world_tile;
	blips_campaign *campaign;
} blips_game;

blips_game* blips_game_create(void);
void blips_game_destroy(blips_game *bgame);

/**/

world_tile* blips_game_active_world_tile(blips_game *bgame);

#endif

