/* breakable_type.h */
/* Started 9/14/12 by Tevis Tsai. */

#ifndef _BREAKABLE_TYPE_H
#define _BREAKABLE_TYPE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _STRING_H
#include<string.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

typedef struct breakable_type
{
	int toughness;
	char *br_set_path;
	char *br_type_path;
	spawn_trigger trigger;
	int respawn;
	int despawn_delay;  /* how much time we let it stick around while it breaks */
} breakable_type;

breakable_type* breakable_type_create(char *path);
void breakable_type_destroy(breakable_type *br_type);

#endif

