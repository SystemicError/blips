/* projectile.c */
/* Started 9/19/12 by Tevis Tsai */

#include"projectile.h"

projectile* projectile_create(projectile_type *pt)
{
	projectile *pr;
	pr=(projectile*)malloc(sizeof(projectile));
	pr->type=pt;
	return pr;
}

void projectile_destroy(projectile *pr)
{
	free(pr);
	return;
}

