/* collectible.c */
/* Started 9/19/12 by Tevis Tsai */

#include"collectible.h"

collectible* collectible_create(collectible_type *ct)
{
	collectible *co;
	if(!(co=(collectible*)malloc(sizeof(collectible))))
	{
		fprintf(stderr,"Couldn't allocate memory for collectible.\n");
		exit(1);
	}
	co->type=ct;

	co->stand_animation_index=0;

	return co;
}

void collectible_destroy(collectible *co)
{
	if(!co)
	{
		fprintf(stderr,"Attempt to destroy null collectible!\n");
		exit(1);
	}
	free(co);
	return;
}

