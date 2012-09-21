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
	for(i=0;i<bgui->num_br_sets;i++)
		breakable_media_set_destroy(bgui->br_sets[i]);
	if(bgui->num_br_sets)
		free(bgui->br_sets);

	for(i=0;i<bgui->num_co_sets;i++)
		collectible_media_set_destroy(bgui->co_sets[i]);
	if(bgui->num_co_sets)
		free(bgui->co_sets);

	for(i=0;i<bgui->num_cr_sets;i++)
		creature_media_set_destroy(bgui->cr_sets[i]);
	if(bgui->num_cr_sets)
		free(bgui->cr_sets);

	for(i=0;i<bgui->num_pr_sets;i++)
		projectile_media_set_destroy(bgui->pr_sets[i]);
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

	/* blips_game reads all the campaign files and keeps the filepath information
	 * on hand, so we didn't have to re-read it here.  In particular, world files
	 * and creature types have already been loaded. */

	/* The tile key content is never read by blips_game, but its filename is. */

	FILE *fp;
	char path[BUFFER_SIZE];
	int i;

	/*** Image Tile Key ***/

printf("Opening tile image key . . .\n");
	if(!(fp=fopen(bgui->game->campaign->tile_image_key_path,"r")))
	{
		fprintf(stderr,"Couldn't open tile_image_key:  %s.\n",bgui->game->campaign->tile_image_key_path);
		exit(1);
	}

	fgets(path,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(bgui->num_tile_images));

	bgui->tile_images=(cairo_surface_t**)malloc(sizeof(cairo_surface_t*)*bgui->num_tile_images);
	bgui->tile_key=(char**)malloc(sizeof(char*)*bgui->num_tile_images);

	fgets(path,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<bgui->num_tile_images;i++)
	{
		bgui->tile_key[i]=(char*)malloc(sizeof(char)*3);
		bgui->tile_key[i][0]=fgetc(fp);
		bgui->tile_key[i][1]=fgetc(fp);
		bgui->tile_key[i][2]=0;  /* null terminator */
		fscanf(fp,"=%s\n",path);
printf("Loading tile image of path:  %s.\n",path);
		bgui->tile_images[i]=cairo_image_surface_create_from_png(path);
	}

	fclose(fp);

	/*** Object Media Sets ***/

	blips_gui_load_media_sets(bgui);

	/*** Background Images ***/

	blips_gui_load_background_images(bgui);

	/* finally, set the active world tile string to 0 so it will update next cycle */
	bgui->active_world_tile_path[0]=0;

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

	if(bgui->num_background_images)
		bgui->active_background=bgui->background_images[blips_gui_string_to_pointer_index(bgui->active_world_tile_path,bgui->background_key,bgui->num_background_images)];

	/* update the active tiles */
printf("Updating active tiles . . .\n");
	tile_string[2]=0;
	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			tile_string[0]=blips_game_active_world_tile(bgui->game)->tile_strings[i][j][0];
			tile_string[1]=blips_game_active_world_tile(bgui->game)->tile_strings[i][j][1];
			bgui->active_tiles[i][j]=bgui->tile_images[blips_gui_string_to_pointer_index(tile_string,bgui->tile_key,bgui->num_tile_images)];
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

	/*** Breakable Media Sets ***/

	bgui->num_br_sets=bgui->game->num_br_types;

	bgui->br_sets=(breakable_media_set**)malloc(sizeof(breakable_media_set*)*bgui->num_br_sets);
	bgui->br_key=(char**)malloc(sizeof(char*)*bgui->num_br_sets);

	for(i=0;i<bgui->game->num_br_types;i++)
	{
		bgui->br_sets[i]=breakable_media_set_create(bgui->game->br_types[i]->br_set_path);
		bgui->br_key[i]=bgui->game->br_types[i]->br_set_path;  /* not a copy, just a pointer */
	}

	/* sort that mess */
	blips_gui_sort_pointers_by_strings((void**)(bgui->br_sets),bgui->br_key,bgui->num_br_sets);

	/*** Collectible Media Sets ***/

	bgui->num_co_sets=bgui->game->num_co_types;

	bgui->co_sets=(collectible_media_set**)malloc(sizeof(collectible_media_set*)*bgui->num_co_sets);
	bgui->co_key=(char**)malloc(sizeof(char*)*bgui->num_co_sets);

	for(i=0;i<bgui->game->num_co_types;i++)
	{
		bgui->co_sets[i]=collectible_media_set_create(bgui->game->co_types[i]->co_set_path);
		bgui->co_key[i]=bgui->game->co_types[i]->co_set_path;  /* not a copy, just a pointer */
	}

	/* sort that mess */
	blips_gui_sort_pointers_by_strings((void**)(bgui->co_sets),bgui->co_key,bgui->num_co_sets);

	/*** Creature Media Sets ***/

	bgui->num_cr_sets=bgui->game->num_cr_types;

	bgui->cr_sets=(creature_media_set**)malloc(sizeof(creature_media_set*)*bgui->num_cr_sets);
	bgui->cr_key=(char**)malloc(sizeof(char*)*bgui->num_cr_sets);

	for(i=0;i<bgui->game->num_cr_types;i++)
	{
		bgui->cr_sets[i]=creature_media_set_create(bgui->game->cr_types[i]->cr_set_path);
		bgui->cr_key[i]=bgui->game->cr_types[i]->cr_set_path;  /* not a copy, just a pointer */
	}

	/* sort that mess */
	blips_gui_sort_pointers_by_strings((void**)(bgui->cr_sets),bgui->cr_key,bgui->num_cr_sets);

	/*** Projectile Media Sets ***/

	bgui->num_pr_sets=bgui->game->num_pr_types;

	bgui->pr_sets=(projectile_media_set**)malloc(sizeof(projectile_media_set*)*bgui->num_pr_sets);
	bgui->pr_key=(char**)malloc(sizeof(char*)*bgui->num_pr_sets);

	for(i=0;i<bgui->game->num_pr_types;i++)
	{
		bgui->pr_sets[i]=projectile_media_set_create(bgui->game->pr_types[i]->pr_set_path);
		bgui->pr_key[i]=bgui->game->pr_types[i]->pr_set_path;  /* not a copy, just a pointer */
	}

	/* sort that mess */
	blips_gui_sort_pointers_by_strings((void**)(bgui->pr_sets),bgui->pr_key,bgui->num_pr_sets);

	return;
}


void blips_gui_load_background_images(blips_gui *bgui)
{
	int i,j,updated,duplicate;
	char *chtmp;
	cairo_surface_t *surftmp;

	/* load bg images from game world tiles */

	bgui->num_background_images=0;
	for(i=0;i<bgui->game->num_world_tiles;i++)
	{
		if(strcmp(bgui->game->world_tiles[i]->background_image,"none"))
		{
			for(j=0;j<bgui->num_background_images;j++)
				if(!strcmp(bgui->game->world_tiles[i]->background_image,bgui->background_key[j]))
				{
					/* This is a duplicate; skip it. */
					j=bgui->num_background_images;
					duplicate=1;
				}
			if(!duplicate)
			{
				/* Something new!  Add it to both the image list and key list. */
					/* make room */
				bgui->background_images=(cairo_surface_t**)realloc(bgui->background_images,sizeof(cairo_surface_t*)*(bgui->num_background_images+1));
				bgui->background_key=(char**)realloc(bgui->background_key,sizeof(char*)*(bgui->num_background_images+1));
					/* copy data */
				bgui->background_images[bgui->num_background_images]=cairo_image_surface_create_from_png(bgui->game->world_tiles[i]->background_image);
				bgui->background_key[bgui->num_background_images]=bgui->game->world_tiles[i]->background_image;
					/* increase count */
				bgui->num_background_images++;
			}
		}
	}

	/* sort bg images by file name */
	blips_gui_sort_pointers_by_strings((void**)(bgui->background_images),bgui->background_key,bgui->num_background_images);
printf("Bg key (should have no duplicates):\n");
for(i=0;i<bgui->num_background_images;i++)
printf("%s\n",bgui->background_key[i]);

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
		{
			cairo_set_source_surface(cr,bgui->active_tiles[i][j],j*BLIPS_TILE_SIZE,i*BLIPS_TILE_SIZE);
			cairo_paint(cr);
		}
	return;
}

void blips_gui_render_objects(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface)
{
	int i;
	int set_index;

	/*** Breakables ***/

	for(i=0;i<bgui->game->num_breakables;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		set_index=blips_gui_string_to_pointer_index(bgui->game->breakables[i]->type->br_type_path,bgui->br_key,bgui->num_br_sets);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_breakable(bgui,cr,surface,bgui->br_sets[set_index],bgui->game->breakables[i]);
	}

	/*** Collectibles ***/

	for(i=0;i<bgui->game->num_collectibles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		set_index=blips_gui_string_to_pointer_index(bgui->game->collectibles[i]->type->co_type_path,bgui->co_key,bgui->num_co_sets);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_collectible(bgui,cr,surface,bgui->co_sets[set_index],bgui->game->collectibles[i]);
	}

	/*** Creatures ***/

	for(i=0;i<bgui->game->num_creatures;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		set_index=blips_gui_string_to_pointer_index(bgui->game->creatures[i]->type->cr_type_path,bgui->cr_key,bgui->num_cr_sets);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_creature(bgui,cr,surface,bgui->cr_sets[set_index],bgui->game->creatures[i]);
	}

	/*** Projectiles ***/

	for(i=0;i<bgui->game->num_projectiles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		set_index=blips_gui_string_to_pointer_index(bgui->game->projectiles[i]->type->pr_type_path,bgui->pr_key,bgui->num_pr_sets);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_projectile(bgui,cr,surface,bgui->pr_sets[set_index],bgui->game->projectiles[i]);
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

/* translation */
int blips_gui_string_to_pointer_index(char *string,char **string_array,int count)
{
	/*** This function is a generic binary search algorithm.  It will be
	 *** applied to any field of blips_gui with a key, accepting string_array
	 *** as the key, count as the number of strings/pointers, and pointers as
	 *** the list of objects corresponding to the key. */

	int upper,lower,middle,comparison;

	if(count<=0)
	{
		fprintf(stderr,"Got binary search request on array of non-positive size!\n");
		exit(1);
	}

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

void blips_gui_sort_pointers_by_strings(void **ptrs,char **strings,int size)
{
	int i,j;
	void *ptmp;
	char *chtmp;

	for(i=0;i<size-1;i++)
		for(j=i;j<size;j++)
			if(strcmp(strings[i],strings[j])>0)
			{
				/* switch 'em */
				ptmp=ptrs[i];
				ptrs[i]=ptrs[j];
				ptrs[j]=ptmp;

				chtmp=strings[i];
				strings[i]=strings[j];
				strings[j]=chtmp;
			}
	return;
}

