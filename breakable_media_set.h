/* breakable_media_set.h */
/* Started 9/17/12 by Tevis Tsai. */

#ifndef _BREAKABLE_MEDIA_SET_H
#define _BREAKABLE_MEDIA_SET_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _SDL_MIXER_H
#include<SDL_mixer.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _SPRITE_ANIMATION_H
#include"sprite_animation.h"
#endif

typedef struct breakable_media_set
{
	sprite_animation *stand_animation;
	sprite_animation *break_animation;
	Mix_Chunk *break_sound;
} breakable_media_set;

breakable_media_set* breakable_media_set_create(char *path);
void breakable_media_set_destroy(breakable_media_set *br_set);

#endif

