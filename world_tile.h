/* world_tile.h */
/* Started 9/18/12 by Tevis Tsai */

#ifndef _WORLD_TILE_H
#define _WORLD_TILE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
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

#ifndef _MAZE_H
#include"maze.h"
#endif

typedef struct world_tile
{
	maze **creature_barriers;
	maze **projectile_barriers;
} world_tile;

world_tile* world_tile_create(char *path);
void world_tile_destroy(world_tile *wt);

#endif

