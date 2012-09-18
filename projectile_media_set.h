/* projectile_media_set.h */
/* Started 9/18/12 by Tevis Tsai. */

#ifndef _PROJECTILE_MEDIA_SET_H
#define _PROJECTILE_MEDIA_SET_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _SDL_MIXER_H
#include<SDL_mixer.h>
#endif

typedef struct projectile_media_set
{
	sprite_animation *fly_animation;
	sprite_animation *impact_animation;
	Mix_Chunk *impact_sound;
} projectile_media_set;

projectile_media_set* projectile_media_set_new(char *path);
void projectile_media_set_destroy(projectile_media_set *pr_set);

#endif

