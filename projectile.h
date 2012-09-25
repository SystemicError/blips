/* projectile.h */
/* Started 9/19/12 by Tevis Tsai */

#ifndef _PROJECTILE_H
#define _PROJECTILE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _PROJECTILE_TYPE_H
#include"projectile_type.h"
#endif

typedef struct projectile
{
	int row,col;
	double x_in_cell,y_in_cell;
	int current_damage;  /* negative for countdown to removal/impacting */
	double orientation;
	int team;
	projectile_type *type;

	int fly_animation_index;
	int impact_animation_index;
} projectile;

projectile* projectile_create(projectile_type *ct);
void projectile_destroy(projectile *cr);

double projectile_absolute_x(projectile *pr);
double projectile_absolute_y(projectile *pr);

#endif

