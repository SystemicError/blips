/* string_map.h */
/* Started 9/21/12 by Tevis Tsai. */

#ifndef _STRING_MAP_H
#define _STRING_MAP_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

typedef struct string_map
{
	void **pointers;  /* not copies!  Just pointers.  */
	char **strings;
	int size;
} string_map;

string_map* string_map_create(void);
void string_map_destroy(string_map *smap);

/* public functions */

void string_map_add(string_map *smap,char *string,void *ptr);

void string_map_string_to_pointer(string_map *smap,char *string,void **ret);

/* private functions */

void string_map_sort(string_map *smap);

#endif

