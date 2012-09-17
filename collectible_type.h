/* collectible_type.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _COLLECTIBLE_TYPE_H
#define _COLLECTIBLE_TYPE_H 1

typedef struct collectible_type
{
	char *co_set_path;
	char *co_type_path;
} collectible_type;

collectible_type* collectible_type_new(char *path);
void collectible_type_destroy(collectible_type *co_type);

#endif

