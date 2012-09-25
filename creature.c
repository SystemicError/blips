/* creature.c */
/* Started 9/19/12 by Tevis Tsai */

#include"creature.h"

creature* creature_create(creature_type *ct)
{
	creature *cr;
	cr=(creature*)malloc(sizeof(creature));
	cr->x_in_cell=BLIPS_TILE_SIZE/2.0;
	cr->y_in_cell=BLIPS_TILE_SIZE/2.0;
	cr->current_health=ct->max_health;
	cr->fire_cycle_state=-1;
	cr->stun_count=0;
	cr->type=ct;
	return cr;
}

void creature_destroy(creature *cr)
{
	free(cr);
	return;
}

double creature_absolute_x(creature *cr)
{
	return cr->col*BLIPS_TILE_SIZE+cr->x_in_cell;
}

double creature_absolute_y(creature *cr)
{
	return cr->row*BLIPS_TILE_SIZE+cr->y_in_cell;
}

