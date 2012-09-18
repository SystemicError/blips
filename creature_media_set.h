/* creature_media_set.h */
/* Started 9/18/12 by Tevis Tsai. */

#ifndef _CREATURE_MEDIA_SET_H
#define _CREATURE_MEDIA_SET_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _SDL_MIXER_H
#include<SDL_mixer.h>
#endif

#ifndef _SPRITE_ANIMATION_H
#include"sprite_animation.h"
#endif

typedef struct creature_media_set
{
	sprite_animation *stand_animation;
	sprite_animation *walk_animation;
	sprite_animation *walk_and_fire_animation;
	sprite_animation *fire_animation;
	sprite_animation *stun_animation;
	Mix_Chunk *stand_sound;
	Mix_Chunk *walk_sound;
	Mix_Chunk *walk_and_fire_sound;
	Mix_Chunk *fire_sound;
	Mix_Chunk *stun_sound;
} creature_media_set;

creature_media_set* creature_media_set_new(char *path);
void creature_media_set_destroy(creature_media_set *cr_set);

#endif

