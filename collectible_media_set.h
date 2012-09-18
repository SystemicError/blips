/* collectible_media_set.h */
/* Started 9/18/12 by Tevis Tsai. */

#ifndef _COLLECTIBLE_MEDIA_SET_H
#define _COLLECTIBLE_MEDIA_SET_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _SDL_MIXER_H
#include<SDL_mixer.h>
#endif

typedef struct collectible_media_set
{
	sprite_animation *stand_animation;
	Mix_Chunk *pickup_sound;
} collectible_media_set;

collectible_media_set* collectible_media_set_new(char *path);
void collectible_media_set_destroy(collectible_media_set *co_set);

#endif

