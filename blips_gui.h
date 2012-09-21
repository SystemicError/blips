/* blips_gui.h */
/* Started 9/14/12 by Tevis Tsai */

#ifndef _BLIPS_GUI_H
#define _BLIPS_GUI_H 1

#ifndef _SDL_H
#include<SDL.h>
#endif

#ifndef _SDL_IMAGE_H
#include<SDL_image.h>
#endif

#ifndef _SDL_MIXER_H
#include<SDL_mixer.h>
#endif

#ifndef _CAIRO_H
#include<cairo.h>
#endif

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _STRING_H
#include<string.h>
#endif

#ifndef _TIME_H
#include<time.h>
#endif

#ifndef _MATH_H
#include<math.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _STRING_MAP_H
#include"string_map.h"
#endif

#ifndef _BLIPS_GAME_H
#include"blips_game.h"
#endif

#ifndef _BREAKABLE_MEDIA_SET_H
#include"breakable_media_set.h"
#endif

#ifndef _COLLECTIBLE_MEDIA_SET_H
#include"collectible_media_set.h"
#endif

#ifndef _CREATURE_MEDIA_SET_H
#include"creature_media_set.h"
#endif

#ifndef _PROJECTILE_MEDIA_SET_H
#include"projectile_media_set.h"
#endif

#ifndef _BLIPS_INPUT_STATE_H
#include"blips_input_state.h"
#endif

typedef struct blips_gui
{
	/*** variables initialized here in blips_gui ***/

	/* SDL main screen */
	SDL_Surface *screen;

	/* Joysticks */
	SDL_Joystick *joy;
	int num_joys;

	/* game data */
	blips_game *game;

	/* media sets */
	string_map *br_map;
	string_map *co_map;
	string_map *cr_map;
	string_map *pr_map;

	/* tile image cache and key */
	cairo_surface_t **tile_images;
	char **tile_key;
	int num_tile_images;

	/* background image cache */
	cairo_surface_t **background_images;
	char **background_key;  /* a link copy only */
	int num_background_images;

	/* active tile array */
	cairo_surface_t *active_tiles[BLIPS_TILE_ROWS][BLIPS_TILE_COLS];

	/* active background */
	cairo_surface_t *active_background;

	/* active music */
	char active_music_path[BUFFER_SIZE];

	/* active world tile path */
	char active_world_tile_path[BUFFER_SIZE];
} blips_gui;

blips_gui* blips_gui_create(blips_game *bgame);
void blips_gui_destroy(blips_gui* bgui);

/* externally called requests */

void blips_gui_main_loop(blips_gui *bgui);

/* internal load, render, event, and translation functions */

	/* load */
void blips_gui_fill_cache(blips_gui *bgui);  /* reads tile+object keys and initializes wts */
void blips_gui_update_active_world_tile(blips_gui *bgui);
void blips_gui_load_media_sets(blips_gui *bgui);
void blips_gui_load_background_images(blips_gui *bgui);

	/* render */
void blips_gui_render_screen(blips_gui *bgui);
void blips_gui_render_bg(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface);
void blips_gui_render_tiles(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface);
void blips_gui_render_objects(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface);

void blips_gui_render_breakable(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,breakable_media_set *br_set,breakable *br);
void blips_gui_render_collectible(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,collectible_media_set *co_set,collectible *co);
void blips_gui_render_creature(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,creature_media_set *cr_set,creature *creat);
void blips_gui_render_projectile(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,projectile_media_set *pr_set,projectile *pr);

	/* events */
int blips_gui_fetch_inputs(blips_gui *bgui,SDL_Event *event,blips_input_state *inputs);

	/* translation */
int blips_gui_string_to_pointer_index(char *string,char **string_array,int count);
void blips_gui_sort_pointers_by_strings(void **ptrs,char **strings,int size);

#endif

