/* breakable.h */
/* Started 9/19/12 by Tevis Tsai */

#ifndef _BREAKABLE_H
#define _BREAKABLE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _BREAKABLE_TYPE_H
#include"breakable_type.h"
#endif

typedef struct breakable
{
	int row,col;
	int time_remaining;  /* -1 indicates not breaking */
	breakable_type *type;

	int stand_animation_index;
	int break_animation_index;
} breakable;

breakable* breakable_create(breakable_type *bt);
void breakable_destroy(breakable *br);

#endif

