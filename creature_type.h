/* creature_type.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _CREATURE_TYPE_H
#define _CREATURE_TYPE_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

typedef struct creature_type
{
	int max_health;
	char *pr_type_path;
	int move_speed;
	int fire_delay;
	char *ai_type_path;
	char *cr_set_path;
	char *cr_type_path;
} creature_type;

creature_type* creature_type_new(char *path);
void creature_type_destroy(creature_type *cr_type);

#endif

