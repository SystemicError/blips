/* collectible.h */
/* Started 9/19/12 by Tevis Tsai */

#ifndef _COLLECTIBLE_H
#define _COLLECTIBLE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _COLLECTIBLE_TYPE_H
#include"collectible_type.h"
#endif

typedef struct collectible
{
	int row,col;
	collectible_type *type;
} collectible;

collectible* collectible_create(collectible_type *ct);
void collectible_destroy(collectible *co);

#endif

