/* creature.h */
/* Started 9/19/12 by Tevis Tsai */

#ifndef _CREATURE_H
#define _CREATURE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _CREATURE_TYPE_H
#include"creature_type.h"
#endif

typedef struct creature
{
	int row,col;
	int x_in_cell,y_in_cell;
	int current_health,current_move_speed;
	double move_orientation,aim_orientation;
	int fire_cycle_state;
	int team;
	creature_type *type;
} creature;

creature* creature_create(creature_type *ct);
void creature_destroy(creature *cr);

#endif

