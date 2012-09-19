/* collectible_type.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _COLLECTIBLE_TYPE_H
#define _COLLECTIBLE_TYPE_H 1

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

typedef struct collectible_type
{
	char *co_set_path;
	char *co_type_path;
} collectible_type;

collectible_type* collectible_type_create(char *path);
void collectible_type_destroy(collectible_type *co_type);

#endif

