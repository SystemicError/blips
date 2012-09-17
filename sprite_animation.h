/* sprite_animation.h */
/* Started 1/20/10 by Tevis Tsai */

#ifndef _SPRITE_ANIMATION_H
#define _SPRITE_ANIMATION_H 1

#ifndef _SDL_H
#include<SDL.h>
#endif

#ifndef _SDL_IMAGE_H
#include<SDL_image.h>
#endif

#ifndef _CAIRO_H
#include<cairo.h>
#endif

typedef struct sprite_animation
{
	int num_frames;
	int count;
	cairo_surface_t **frames;
} sprite_animation;

sprite_animation* sprite_animation_new(void);

void sprite_animation_destroy(sprite_animation *sa);

cairo_surface_t* sprite_animation_cycle(sprite_animation *sa);

void sprite_animation_add_frame(sprite_animation *sa,const char *path);

#endif

