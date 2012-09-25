/* breakable.c */
/* Started 9/19/12 by Tevis Tsai */

#include"breakable.h"

breakable* breakable_create(breakable_type *bt)
{
	breakable *br;
	br=(breakable*)malloc(sizeof(breakable));
	br->time_remaining=-1;
	br->type=bt;

	br->stand_animation_index=0;
	br->break_animation_index=0;

	return br;
}

void breakable_destroy(breakable *br)
{
	if(!br)
	{
		fprintf(stderr,"Attempt to destroy null breakable!\n");
		exit(1);
	}
	free(br);
	return;
}

