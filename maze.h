/* maze.h */
/* Separated from labyrinth's main.c on 8/16/10 by Tevis Tsai. */
/* Repurposed for Squawk Box on 6/8/12 by Tevis Tsai. */
/* Re-repurposed for Blips on 9/18/12 by Tevis Tsai. */

#ifndef _MAZE_H
#define _MAZE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#define MAZE_NORTH 0
#define MAZE_EAST 1
#define MAZE_SOUTH 2
#define MAZE_WEST 3

typedef struct maze
{
	int rows,columns;
	int **tiles;
} maze;

maze* maze_create(int r,int c);
void maze_destroy(maze *m);

void maze_generate(maze *m);

void maze_place_wall(maze *m,int r,int c,int direction);

void maze_print(maze *m);

int maze_contains_wall(maze *m,int r,int c,int direction);

/* internal use only */

int maze_partioned_by_wall(maze *m,int cell,int direction,int cell2,int direction2);

void maze_get_opposing_link(maze *m,int cell,int direction,int *cell2,int *direction2);

int maze_is_maximal(maze *m);

int minimum_distance_between(maze *m,int start,int finish);

#endif

