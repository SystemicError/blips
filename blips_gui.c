/* blips_gui.c */
/* Started 9/14/12 by Tevis Tsai */

#include"blips_gui.h"

blips_gui* blips_gui_create(blips_game *bgame)
{
	int i;

	blips_gui *bgui;

	printf("Initializing Blips GUI . . .\n");

	bgui=(blips_gui*)malloc(sizeof(blips_gui));

	/*** Video ***/

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0)
	{
		fprintf(stderr,"Couldn't initialize SDL video/joystick.\n");
		exit(1);
	}
	if(!(bgui->screen=SDL_SetVideoMode(1024,768,32,SDL_HWSURFACE)))
	{
		SDL_Quit();
		fprintf(stderr,"Couldn't set video mode.\n");
		exit(1);
	}

	/*** Input ***/
	int num_joys=2;
	for(i=0;i<num_joys;i++)
		if(!(bgui->joy=SDL_JoystickOpen(i)))
			fprintf(stderr,"Couldn't open joystick %d; will not be enabled.\n",i);

	/*** Audio ***/

	if(SDL_Init(SDL_INIT_AUDIO)<0)
	{
		fprintf(stderr,"Couldn't initialize audio.\n");
		exit(1);
	}
	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024)<0)
	{
		fprintf(stderr,"Mix_OpenAudio failed:  %s\n",Mix_GetError());
		exit(1);
	}
	Mix_AllocateChannels(16);

	/*** Other data ***/

	bgui->game=bgame;

	blips_gui_fill_cache(bgui);

	return bgui;
}

void blips_gui_destroy(blips_gui* bgui)
{
	int i;
printf("Blips GUI destructor called:\n");
	if(!bgui)
	{
		fprintf(stderr,"Attempt to destroy null blips_gui!\n");
		exit(1);
	}

printf("Freeing blips game.\n");
	if(bgui->game)
		free(bgui->game);

printf("Freeing media.\n");
	/* media sets */
	for(i=0;i<bgui->num_br_sets;i++)
		breakable_media_set_destroy(bgui->br_sets+i);
	if(bgui->num_br_sets)
		free(bgui->br_sets);

	for(i=0;i<bgui->num_co_sets;i++)
		collectible_media_set_destroy(bgui->co_sets+i);
	if(bgui->num_co_sets)
		free(bgui->co_sets);

	for(i=0;i<bgui->num_cr_sets;i++)
		creature_media_set_destroy(bgui->cr_sets+i);
	if(bgui->num_cr_sets)
		free(bgui->cr_sets);

	for(i=0;i<bgui->num_pr_sets;i++)
		projectile_media_set_destroy(bgui->pr_sets+i);
	if(bgui->num_pr_sets)
		free(bgui->pr_sets);

	/* tile images */
	for(i=0;i<bgui->num_tile_images;i++)
		cairo_surface_destroy(bgui->tile_images[i]);
	if(bgui->tile_images)
		free(bgui->tile_images);

	/* bg images */
	for(i=0;i<bgui->num_background_images;i++)
		cairo_surface_destroy(bgui->background_images[i]);
	if(bgui->background_images)
		free(bgui->background_images);

printf("Closing audio.\n");
	Mix_CloseAudio();
	SDL_JoystickClose(bgui->joy);
	SDL_Quit();
	free(bgui);
	bgui=0;
	return;
}

/* externally called requests */

	/* video playback */

void blips_gui_main_loop(blips_gui *bgui)
{
	SDL_Event event;
	blips_input_state *inputs;
	int i, quit;
	quit=0;

	inputs=blips_input_state_create();
	while(!quit)
	{
		/* display stuff */
		blips_gui_render_screen(bgui);

		/* listen for stuff */
		while(SDL_PollEvent(&event))
			quit=blips_gui_fetch_inputs(bgui,&event,inputs);

		/* check if world tile has changed */
		if(strcmp(bgui->active_world_tile_path,blips_game_active_world_tile(bgui->game)->path))
			blips_gui_update_active_world_tile(bgui);

		/* iterate the game */
		blips_game_step(bgui->game,inputs);
	}
	blips_input_state_destroy(inputs);
	return;
}

/* internal load, render, and event functions */

/* load */

void blips_gui_fill_cache(blips_gui *bgui)
{
	/*** This function will read through
	 *** bgui->game and fill out the caches of images and media sets.
	 *** It is also charged with filling out the key of tiles and backgrounds.*/

	/* How much of this function will be given to blips_game?  It would be nice if blips_game read all the campaign files and kept the information on hand, so we didn't have to re-read it here. */

	/* For now, let's assume that the tile key was never read, but its filename was. */

	FILE *fp;
	char path[1024];
	int i;

	/* begin with the easy stuff, the image tile key */

	fp=fopen(bgui->game->campaign->tile_image_key_path,"r");

	fscanf(fp,"%s\n",path);  /* comment line */
	fscanf(fp,"%d\n",bgui->num_tile_images);

	bgui->tile_images=(cairo_surface_t**)malloc(sizeof(cairo_surface_t*)*bgui->num_tile_images);
	bgui->tile_key=(char**)malloc(sizeof(char*)*bgui->num_tile_images);

	fscanf(fp,"%s\n",path);  /* comment line */
	for(i=0;i<bgui->num_tile_images;i++)
	{
		bgui->tile_key[i]=(char*)malloc(sizeof(char)*2);
		fscanf(fp,"%s=%s\n",bgui->tile_key[i],path);
		bgui->tile_images[i]=cairo_image_surface_create_from_png(path);
	}

	/* UNFINISHED */
bgui->num_br_sets=0;
bgui->num_co_sets=0;
bgui->num_cr_sets=0;
bgui->num_pr_sets=0;
bgui->num_background_images=0;

	/* finally, set the active world tile string to 0 so it will update next cycle */
	bgui->active_world_tile_path[0]=0;

	fclose(fp);
	return;
}

void blips_gui_update_active_world_tile(blips_gui *bgui)
{
	/*** This function is called when the gui has detected that the active
	 *** world tile has changed.  The function must now change the recorded
	 *** active tile and update the active tile pointers and background
	 *** pointer.  This way, the time-intensive tasks of translating tile 
	 *** strings and background paths to pointers in the image cache are called
	 *** only once for each world tile. */

	int i,j;

	/* update the active world tile path */
	strcpy(bgui->active_world_tile_path,blips_game_active_world_tile(bgui->game)->path);

	/* update the active background */
	bgui->active_background=bgui->background_images[blips_gui_string_to_pointer_index(bgui->active_world_tile_path,bgui->background_key,bgui->num_background_images)];

	/* update the active tiles */
	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
			bgui->active_tiles[i][j]=bgui->tile_images[blips_gui_string_to_pointer_index(blips_game_active_world_tile(bgui->game)->tile_strings[i][j],bgui->tile_key,bgui->num_tile_images)];
	return;
}

/* render */

void blips_gui_render_screen(blips_gui *bgui)
{
	cairo_t *cr;
	cairo_surface_t *surface;
	SDL_Surface *next_screen;

	next_screen=SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,
					 bgui->screen->w,
					 bgui->screen->h,
					 32,
					 0x00FF0000,
					 0x0000FF00,
					 0x000000FF,
					 0);

	if(SDL_MUSTLOCK(next_screen))
		if(SDL_LockSurface(next_screen)<0)
		{
			fprintf(stderr,"Can't lock surface:  %s\n",SDL_GetError());
			exit(1);
		}

	surface=cairo_image_surface_create_for_data((unsigned char*)(next_screen->pixels),
						    CAIRO_FORMAT_RGB24,
						    next_screen->w,
						    next_screen->h,
						    next_screen->pitch);

	cr=cairo_create(surface);

	blips_gui_render_bg(bgui,cr,surface);
	blips_gui_render_objects(bgui,cr,surface);

	cairo_surface_destroy(surface);
	cairo_destroy(cr);

	SDL_BlitSurface(next_screen,NULL,bgui->screen,NULL);

	if(SDL_MUSTLOCK(next_screen))
		SDL_UnlockSurface(next_screen);

	SDL_FreeSurface(next_screen);

	SDL_UpdateRect(bgui->screen,0,0,bgui->screen->w,bgui->screen->h);
	return;
}

void blips_gui_render_bg(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface)
{
	cairo_pattern_t *pattern;

	/* if bg is not set for active world tile, leave */
	if(!(bgui->active_background))
		return;

	pattern=cairo_pattern_create_for_surface(bgui->active_background);
	cairo_pattern_set_extend(pattern,CAIRO_EXTEND_REPEAT);

	cairo_set_source(cr,pattern);
	cairo_paint(cr);

	cairo_pattern_destroy(pattern);

	return;
}

void blips_gui_render_objects(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface)
{
	/* UNFINISHED */

	return;
}

/* events */

int blips_gui_fetch_inputs(blips_gui *bgui,SDL_Event *event,blips_input_state *inputs)
{
	/*** This function will translate SDL events into a blips_input_state,
	 *** which the gui will use to inform the game of events while hiding
	 *** how they are obtained. ***/
	int i;

	/* modify button/joystick states as necessary */
	switch(event->type)
	{
		case SDL_QUIT:
			return 1;
		break;
		case SDL_JOYBUTTONDOWN:
/* UNFINISHED */
		break;
		case SDL_JOYBUTTONUP:
		break;
	}
	return 0;
}

/* translation */
int blips_gui_string_to_pointer_index(char *string,char **string_array,int count)
{
	/*** This function is a generic binary search algorithm.  It will be
	 *** applied to any field of blips_gui with a key, accepting string_array
	 *** as the key, count as the number of strings/pointers, and pointers as
	 *** the list of objects corresponding to the key. */

	int upper,lower,middle,comparison;
	upper=count;
	lower=0;
	middle=(lower+upper)/2;

	while(comparison=strcmp(string,string_array[middle]))
	{
		if(comparison>0)  /* input>candidate */
			lower=middle;
		else
			upper=middle;

		if(lower==upper)
		{
			fprintf(stderr,"Got request for item not in key!\n");
			exit(1);
		}

		middle=(lower+upper)/2;
	}
	return middle;
}

