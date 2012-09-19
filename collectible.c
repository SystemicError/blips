/* collectible.c */
/* Started 9/19/12 by Tevis Tsai */

#include"collectible.h"

collectible* collectible_create(collectible_type *ct)
{
	collectible *co;
	co=(collectible*)malloc(sizeof(collectible));
	co->type=ct;
	return co;
}

void collectible_destroy(collectible *co)
{
	free(co);
	return;
}

