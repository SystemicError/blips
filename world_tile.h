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

#ifndef _STRING_H
#include<string.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
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

#define BLIPS_TILE_ROWS 11
#define BLIPS_TILE_COLS 16
#define BLIPS_TILE_SIZE 64

typedef struct world_tile
{
	char object_strings[BLIPS_TILE_ROWS][BLIPS_TILE_COLS][2];
	maze *creature_barriers;
	maze *projectile_barriers;
	char tile_strings[BLIPS_TILE_ROWS][BLIPS_TILE_COLS][2];
	char *background_image;
	char *north_tile;
	char *east_tile;
	char *south_tile;
	char *west_tile;
	char *music;
	char *path;  /* the path to this world tile */
} world_tile;

world_tile* world_tile_create(char *path);
void world_tile_destroy(world_tile *wt);

/* internal helper functions */

void world_tile_parse_barriers(FILE *fp,maze *m);

#endif

