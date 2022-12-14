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
	double x_in_cell,y_in_cell;
	int current_health,current_move_speed;
	double move_orientation,aim_orientation;
	int fire_cycle_state;  /* -1 indicates not firing. */
	int stun_count;  /* 0 indicates not stunned. */
	int team;
	creature_type *type;

	int stand_animation_index;
	int walk_animation_index;
	int walk_and_fire_animation_index;
	int fire_animation_index;
	int stun_animation_index;
} creature;

creature* creature_create(creature_type *ct);
void creature_destroy(creature *cr);

double creature_absolute_x(creature *cr);
double creature_absolute_y(creature *cr);

#endif

