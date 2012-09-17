/* projectile_type.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _PROJECTILE_TYPE_H
#define _PROJECTILE_TYPE_H 1

typedef struct projectile_type
{
	int damage;
	int move_speed;
	char *pr_set_path;
	char *pr_type_path;
} projectile_type;

projectile_type* projectile_type_new(char *path);
void projectile_type_destroy(projectile_type *pr_type);

#endif

