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

printf("Initializing audio . . .\n");
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

	/* object media sets and keys */

	bgui->br_map=string_map_create();
	bgui->co_map=string_map_create();
	bgui->cr_map=string_map_create();
	bgui->pr_map=string_map_create();

	/* background map key */

	bgui->background_map=string_map_create();

	/* tile map key */

	bgui->tile_map=string_map_create();

printf("Filling cache . . .\n");
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

printf("Freeing media.\n");
	/* media sets */
	for(i=0;i<bgui->br_map->size;i++)
		breakable_media_set_destroy((breakable_media_set*)(bgui->br_map->pointers[i]));
	string_map_destroy(bgui->br_map);

	for(i=0;i<bgui->co_map->size;i++)
		collectible_media_set_destroy((collectible_media_set*)(bgui->co_map->pointers[i]));
	string_map_destroy(bgui->co_map);

	for(i=0;i<bgui->cr_map->size;i++)
		creature_media_set_destroy((creature_media_set*)(bgui->cr_map->pointers[i]));
	string_map_destroy(bgui->cr_map);

	for(i=0;i<bgui->pr_map->size;i++)
		projectile_media_set_destroy((projectile_media_set*)(bgui->pr_map->pointers[i]));
	string_map_destroy(bgui->pr_map);

	/* tile images -- somewhat unique in that the strings are instanced here, not elsewhere */
	for(i=0;i<bgui->tile_map->size;i++)
	{
		free(bgui->tile_map->strings[i]);
		cairo_surface_destroy((cairo_surface_t*)(bgui->tile_map->pointers[i]));
	}
	string_map_destroy(bgui->tile_map);

	/* bg images */
	for(i=0;i<bgui->background_map->size;i++)
		cairo_surface_destroy((cairo_surface_t*)(bgui->background_map->pointers[i]));
	string_map_destroy(bgui->background_map);


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

	/* blips_game reads all the campaign files and keeps the filepath information
	 * on hand, so we didn't have to re-read it here.  In particular, world files
	 * and creature types have already been loaded. */

	/* The tile key content is never read by blips_game, but its filename is. */

	FILE *fp;
	char path[BUFFER_SIZE];
	int i,count;
	char *chs;
	cairo_surface_t *surface;

	/*** Image Tile Key ***/

	if(!(fp=fopen(bgui->game->campaign->tile_image_key_path,"r")))
	{
		fprintf(stderr,"Couldn't open tile_image_key:  %s.\n",bgui->game->campaign->tile_image_key_path);
		exit(1);
	}

	fgets(path,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&count);

	fgets(path,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		chs=(char*)malloc(sizeof(char)*3);
		chs[0]=fgetc(fp);
		chs[1]=fgetc(fp);
		chs[2]=0;  /* null terminator */
		fscanf(fp,"=%s\n",path);
printf("Reading tile: %s.\n",path);
		surface=cairo_image_surface_create_from_png(path);

		/* Error check */
		switch(cairo_surface_status(surface))
		{
			case CAIRO_STATUS_NO_MEMORY:
				fprintf(stderr,"Error loading sprite animation frame (no memory available):  %s\n",path);
				exit(1);
			break;
			case CAIRO_STATUS_FILE_NOT_FOUND:
				fprintf(stderr,"Error loading sprite animation frame (file not found):  %s\n",path);
				exit(1);
			break;
			case CAIRO_STATUS_READ_ERROR:
				fprintf(stderr,"Error loading sprite animation frame (read error):  %s\n",path);
				exit(1);
			break;
		}

		string_map_add(bgui->tile_map,chs,surface);
	}

	fclose(fp);

	/*** Object Media Sets ***/

	blips_gui_load_media_sets(bgui);

	/*** Background Images ***/

	blips_gui_load_background_images(bgui);

	/* finally, set the active world tile string to 0 so it will update next cycle */
	bgui->active_world_tile_path[0]=0;
printf("Done filling cache.\n");

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
	char tile_string[3];

printf("Copying new path . . .\n");
	/* update the active world tile path */
	strcpy(bgui->active_world_tile_path,blips_game_active_world_tile(bgui->game)->path);

	/* update the active background */

	if(bgui->background_map->size)
		string_map_string_to_pointer(bgui->background_map,bgui->active_world_tile_path,(void**)&(bgui->active_background));

	/* update the active tiles */
	tile_string[2]=0;
	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			tile_string[0]=blips_game_active_world_tile(bgui->game)->tile_strings[i][j][0];
			tile_string[1]=blips_game_active_world_tile(bgui->game)->tile_strings[i][j][1];
			string_map_string_to_pointer(bgui->tile_map,
						     tile_string,
						     (void**)&(bgui->active_tiles[i][j]));
		}
	return;
}

void blips_gui_load_media_sets(blips_gui *bgui)
{
	/*** Helper function to fill_cache(). ***/
	/* Theoretically inefficient, since we're keying media sets by object types.
	 * If multiple objects have the same media set, we're wasting time searching
	 * through key strings (object type strings) that differ, even though
	 * their content (media sets) do not.  However, there would have to be many,
	 * many different types of identical media set before the performance difference
	 * became noticeable. */

	int i;
	char *path;

	/*** Breakable Media Sets ***/

	for(i=0;i<bgui->game->br_types_map->size;i++)
	{
		path=((breakable_type*)(bgui->game->br_types_map->pointers[i]))->br_set_path;
		string_map_add(bgui->br_map,
				path,
				(void*)breakable_media_set_create(path));
	}

	/*** Collectible Media Sets ***/

	for(i=0;i<bgui->game->co_types_map->size;i++)
	{
		path=((collectible_type*)(bgui->game->co_types_map->pointers[i]))->co_set_path;
		string_map_add(bgui->co_map,
				path,
				(void*)collectible_media_set_create(path));
	}

	/*** Creature Media Sets ***/

	for(i=0;i<bgui->game->cr_types_map->size;i++)
	{
		path=((creature_type*)(bgui->game->cr_types_map->pointers[i]))->cr_set_path;
		string_map_add(bgui->cr_map,
				path,
				(void*)creature_media_set_create(path));
	}

	/*** Projectile Media Sets ***/

	for(i=0;i<bgui->game->num_pr_types;i++)
		string_map_add(bgui->pr_map,
				bgui->game->pr_types[i]->pr_set_path,
				(void*)projectile_media_set_create(bgui->game->pr_types[i]->pr_set_path));

	return;
}


void blips_gui_load_background_images(blips_gui *bgui)
{
	int i,j,updated,duplicate;
	char *chtmp;
	cairo_surface_t *surftmp;

	/* load bg images from game world tiles */

	for(i=0;i<bgui->game->num_world_tiles;i++)
	{
		if(strcmp(bgui->game->world_tiles[i]->background_image,"none"))
		{
			for(j=0;j<bgui->background_map->size;j++)
				if(!strcmp(bgui->game->world_tiles[i]->background_image,bgui->background_map->strings[j]))
				{
					/* This is a duplicate; skip it. */
					j=bgui->background_map->size;
					duplicate=1;
				}
			if(!duplicate)
			{
				/* Something new!  Add it to both the image list and key list. */
				string_map_add(bgui->background_map,
					       bgui->game->world_tiles[i]->background_image,
					       (void*)cairo_image_surface_create_from_png(bgui->game->world_tiles[i]->background_image));
			}
		}
	}

	return;
}

/*** render ***/

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
	blips_gui_render_tiles(bgui,cr,surface);
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

	/* if bg is not set for active world tile, skip to tiles images */
	if(!(bgui->active_background))
		return;

	pattern=cairo_pattern_create_for_surface(bgui->active_background);
	cairo_pattern_set_extend(pattern,CAIRO_EXTEND_REPEAT);

	cairo_set_source(cr,pattern);
	cairo_paint(cr);

	cairo_pattern_destroy(pattern);

	return;
}

void blips_gui_render_tiles(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface)
{
	int i,j;

	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
			if(bgui->active_tiles[i][j])
			{
				cairo_set_source_surface(cr,bgui->active_tiles[i][j],j*BLIPS_TILE_SIZE,i*BLIPS_TILE_SIZE);
				cairo_paint(cr);
			}
	return;
}

void blips_gui_render_objects(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface)
{
	int i;
	void *ptr;

	/*** Breakables ***/

	for(i=0;i<bgui->game->num_breakables;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->br_map,bgui->game->breakables[i]->type->br_type_path,ptr);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_breakable(bgui,cr,surface,
					   (breakable_media_set*)ptr,
					   bgui->game->breakables[i]);
	}

	/*** Collectibles ***/

	for(i=0;i<bgui->game->num_collectibles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->co_map,bgui->game->collectibles[i]->type->co_type_path,ptr);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_collectible(bgui,cr,surface,
					     (collectible_media_set*)ptr,
					     bgui->game->collectibles[i]);
	}

	/*** Creatures ***/

	for(i=0;i<bgui->game->num_creatures;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->cr_map,bgui->game->creatures[i]->type->cr_type_path,ptr);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_creature(bgui,cr,surface,
					  (creature_media_set*)ptr,
					  bgui->game->creatures[i]);
	}

	/*** Projectiles ***/

	for(i=0;i<bgui->game->num_projectiles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->pr_map,bgui->game->projectiles[i]->type->pr_type_path,ptr);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_projectile(bgui,cr,surface,
					    (projectile_media_set*)ptr,
					     bgui->game->projectiles[i]);
	}

	return;
}

void blips_gui_render_breakable(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,breakable_media_set *br_set,breakable *br)
{
	/* UNFINISHED */
	return;
}

void blips_gui_render_collectible(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,collectible_media_set *co_set,collectible *co)
{
	/* UNFINISHED */
	return;
}

void blips_gui_render_creature(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,creature_media_set *cr_set,creature *creat)
{
	/* UNFINISHED */
	return;
}

void blips_gui_render_projectile(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,projectile_media_set *pr_set,projectile *pr)
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

