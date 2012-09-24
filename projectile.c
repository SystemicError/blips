/* projectile.c */
/* Started 9/19/12 by Tevis Tsai */

#include"projectile.h"

projectile* projectile_create(projectile_type *pt)
{
	projectile *pr;
	pr=(projectile*)malloc(sizeof(projectile));
	pr->current_damage=pt->damage;
	pr->type=pt;
	return pr;
}

void projectile_destroy(projectile *pr)
{
	free(pr);
	return;
}

double projectile_absolute_x(projectile *pr)
{
	return pr->col*BLIPS_TILE_SIZE+pr->x_in_cell;
}

double projectile_absolute_y(projectile *pr)
{
	return pr->row*BLIPS_TILE_SIZE+pr->y_in_cell;
}

