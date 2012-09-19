/* breakable.c */
/* Started 9/19/12 by Tevis Tsai */

#include"breakable.h"

breakable* breakable_create(breakable_type *bt)
{
	breakable *br;
	br=(breakable*)malloc(sizeof(breakable));
	br->type=bt;
	return br;
}

void breakable_destroy(breakable *br)
{
	free(br);
	return;
}

