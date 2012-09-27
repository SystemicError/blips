/* creature.c */
/* Started 9/19/12 by Tevis Tsai */

#include"creature.h"

creature* creature_create(creature_type *ct)
{
	creature *cr;
	if(!(cr=(creature*)malloc(sizeof(creature))))
	{
		fprintf(stderr,"Couldn't allocate memory for creature.\n");
		exit(1);
	}
	cr->x_in_cell=BLIPS_TILE_SIZE/2.0;
	cr->y_in_cell=BLIPS_TILE_SIZE/2.0;
	cr->current_health=ct->max_health;
	cr->current_move_speed=0;
	cr->move_orientation=0;
	cr->aim_orientation=0;
	cr->fire_cycle_state=-1;
	cr->stun_count=0;
	cr->type=ct;

	cr->stand_animation_index=0;
	cr->walk_animation_index=0;
	cr->walk_and_fire_animation_index=0;
	cr->fire_animation_index=0;
	cr->stun_animation_index=0;

	return cr;
}

void creature_destroy(creature *cr)
{
	if(!cr)
	{
		fprintf(stderr,"Attempt to free null creature!\n");
		exit(1);
	}
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

