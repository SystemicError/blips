/* ai_type.h */
/* Started 9/17/12 by Tevis Tsai. */
/* The only type without an associated media set! */

#ifndef _AI_TYPE_H
#define _AI_TYPE_H 1

typedef enum ai_move_orientation_goal {AI_DODGE,ADI_FLEE,AI_CLOSE,AI_WANDER,AI_PATROL_NS,AI_PATROL_EW} ai_move_orientation_goal;
typedef enum ai_aim_orientation_goal {AI_FACE,AI_SPIN,AI_LAG_FACE,AI_LEAD_FACE,AI_AS_VELOCITY} ai_aim_orientation_goal;
typedef enum ai_fire_goal {AI_SPAM,AI_SIGHT_FIRST,AI_SPURT} ai_fire_goal;

typedef struct ai_type
{
	ai_move_orientation_goal move_goal;
	ai_aim_orientation_goal aim_goal;
	ai_fire_goal fire_goal;
	char *ai_type_path;
} ai_type;

ai_type* ai_type_new(char *path);
void ai_type_destroy(ai_type *ai_type);

#endif

