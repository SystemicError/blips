/* creature.c */
/* Started 9/19/12 by Tevis Tsai */

#include"creature.h"

creature* creature_create(creature_type *ct)
{
	creature *cr;
	cr=(creature*)malloc(sizeof(creature));
	cr->type=ct;
	return cr;
}

void creature_destroy(creature *cr)
{
	free(cr);
	return;
}

