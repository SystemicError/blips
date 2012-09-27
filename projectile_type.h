/* projectile_type.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _PROJECTILE_TYPE_H
#define _PROJECTILE_TYPE_H 1

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

typedef struct projectile_type
{
	int damage;
	int move_speed;
	int despawn_delay;  /* how much time we let it stick around while it impacts */
	char *pr_set_path;
	char *pr_type_path;
} projectile_type;

projectile_type* projectile_type_create(char *path);
void projectile_type_destroy(projectile_type *pr_type);

#endif

