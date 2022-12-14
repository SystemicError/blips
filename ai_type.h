/* ai_type.h */
/* Started 9/17/12 by Tevis Tsai. */
/* The only type without an associated media set! */

#ifndef _AI_TYPE_H
#define _AI_TYPE_H 1

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

typedef enum ai_move_orientation_goal {AI_DODGE,AI_FLEE,AI_CLOSE,AI_WANDER,AI_PATROL_NS,AI_PATROL_EW} ai_move_orientation_goal;
typedef enum ai_aim_orientation_goal {AI_FACE,AI_SPIN,AI_LAG_FACE,AI_LEAD_FACE,AI_AS_VELOCITY} ai_aim_orientation_goal;
typedef enum ai_fire_goal {AI_SPAM,AI_SIGHT_FIRST,AI_SPURT} ai_fire_goal;

typedef struct ai_type
{
	ai_move_orientation_goal move_goal;
	ai_aim_orientation_goal aim_goal;
	ai_fire_goal fire_goal;
	char *ai_type_path;
} ai_type;

ai_type* ai_type_create(char *path);
void ai_type_destroy(ai_type *ai_type);

#endif

