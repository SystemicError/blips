/* projectile.c */
/* Started 9/19/12 by Tevis Tsai */

#include"projectile.h"

projectile* projectile_create(projectile_type *pt)
{
	projectile *pr;
	pr=(projectile*)malloc(sizeof(projectile));
	pr->current_damage=pt->damage;
	pr->type=pt;

	pr->fly_animation_index=0;
	pr->impact_animation_index=0;

	return pr;
}

void projectile_destroy(projectile *pr)
{
	if(!pr)
	{
		fprintf(stderr,"Attempt to free null projectile!\n");
		exit(1);
	}
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

