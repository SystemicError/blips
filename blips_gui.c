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
	bgui->num_joys=2; /* should be configurable */
	bgui->joys=(SDL_Joystick**)malloc(sizeof(SDL_Joystick*)*bgui->num_joys);
	for(i=0;i<bgui->num_joys;i++)
		if(!(bgui->joys[i]=SDL_JoystickOpen(i)))
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

	bgui->player_map=string_map_create();

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

	for(i=0;i<bgui->player_map->size;i++)
		creature_media_set_destroy((creature_media_set*)(bgui->player_map->pointers[i]));
	string_map_destroy(bgui->player_map);

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
	for(i=0;i<bgui->num_joys;i++)
		SDL_JoystickClose(bgui->joys[i]);
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
	blips_input_state **inputs;
	int i, quit;
	quit=0;

	inputs=(blips_input_state**)malloc(sizeof(blips_input_state*)*bgui->game->campaign->num_players);
	for(i=0;i<bgui->game->campaign->num_players;i++)
		inputs[i]=blips_input_state_create();
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
	for(i=0;i<bgui->game->campaign->num_players;i++)
		blips_input_state_destroy(inputs[i]);
	free(inputs);
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
		fgetc(fp);  /* the '=' symbol */
		fgets(path,BUFFER_SIZE,fp);
		path[strlen(path)-1]=0;  /* replace endline with null terminator */
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
	char *set_path;
	char *type_path;

	/*** Breakable Media Sets ***/

	for(i=0;i<bgui->game->br_types_map->size;i++)
	{
		type_path=((breakable_type*)(bgui->game->br_types_map->pointers[i]))->br_type_path;
		set_path=((breakable_type*)(bgui->game->br_types_map->pointers[i]))->br_set_path;
		string_map_add(bgui->br_map,
				type_path,
				(void*)breakable_media_set_create(set_path));
	}

	/*** Collectible Media Sets ***/

	for(i=0;i<bgui->game->co_types_map->size;i++)
	{
		type_path=((collectible_type*)(bgui->game->co_types_map->pointers[i]))->co_type_path;
		set_path=((collectible_type*)(bgui->game->co_types_map->pointers[i]))->co_set_path;
		string_map_add(bgui->co_map,
				type_path,
				(void*)collectible_media_set_create(set_path));
	}

	/*** Creature Media Sets ***/

		/* non-player */
	for(i=0;i<bgui->game->cr_types_map->size;i++)
	{
		type_path=((creature_type*)(bgui->game->cr_types_map->pointers[i]))->cr_type_path;
		set_path=((creature_type*)(bgui->game->cr_types_map->pointers[i]))->cr_set_path;
		string_map_add(bgui->cr_map,
				type_path,
				(void*)creature_media_set_create(set_path));
	}
		/* player */
	for(i=0;i<bgui->game->campaign->num_players;i++)
	{
		type_path=bgui->game->player_types[i]->cr_type_path;
		set_path=bgui->game->player_types[i]->cr_set_path;
		string_map_add(bgui->player_map,
				type_path,
				(void*)creature_media_set_create(set_path));
	}

	/*** Projectile Media Sets ***/

	for(i=0;i<bgui->game->pr_types_map->size;i++)
	{
		type_path=((projectile_type*)(bgui->game->pr_types_map->pointers[i]))->pr_type_path;
		set_path=((projectile_type*)(bgui->game->pr_types_map->pointers[i]))->pr_set_path;
		string_map_add(bgui->pr_map,
				type_path,
				(void*)projectile_media_set_create(set_path));
	}

	return;
}


void blips_gui_load_background_images(blips_gui *bgui)
{
	int i,j,updated,duplicate;
	char *chtmp;
	cairo_surface_t *surftmp;
	world_tile *wt;

	/* load bg images from game world tiles */

	for(i=0;i<bgui->game->world_tile_map->size;i++)
	{
		wt=(world_tile*)(bgui->game->world_tile_map->pointers[i]);
		if(strcmp(wt->background_image,"none"))
		{
			for(j=0;j<bgui->background_map->size;j++)
				if(!strcmp(wt->background_image,bgui->background_map->strings[j]))
				{
					/* This is a duplicate; skip it. */
					j=bgui->background_map->size;
					duplicate=1;
				}
			if(!duplicate)
			{
				/* Something new!  Add it to both the image list and key list. */
				string_map_add(bgui->background_map,
					       wt->background_image,
					       (void*)cairo_image_surface_create_from_png(wt->background_image));
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

if(!strcmp(bgui->game->active_world_tile->path,"example_campaign/world_tiles/example_0x1.world_tile"))
return;

printf("Call to Render.\n");
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
		string_map_string_to_pointer(bgui->br_map,bgui->game->breakables[i]->type->br_type_path,&ptr);

		/* render that media set according to the stat of this object instance */
		if(ptr)
			blips_gui_render_breakable(bgui,cr,surface,
						   (breakable_media_set*)ptr,
						   bgui->game->breakables[i]);
	}

	/*** Collectibles ***/

	for(i=0;i<bgui->game->num_collectibles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->co_map,bgui->game->collectibles[i]->type->co_type_path,&ptr);

		/* render that media set according to the stat of this object instance */
		if(ptr)
			blips_gui_render_collectible(bgui,cr,surface,
						     (collectible_media_set*)ptr,
						     bgui->game->collectibles[i]);
	}

	/*** Creatures ***/

		/* non-player */
	for(i=0;i<bgui->game->num_creatures;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->cr_map,bgui->game->creatures[i]->type->cr_type_path,&ptr);

		/* render that media set according to the stat of this object instance */
		if(ptr)
			blips_gui_render_creature(bgui,cr,surface,
						  (creature_media_set*)ptr,
						  bgui->game->creatures[i]);
	}
		/* player */
	for(i=0;i<bgui->game->campaign->num_players;i++)
	{
		string_map_string_to_pointer(bgui->player_map,bgui->game->players[i]->type->cr_type_path,&ptr);

		blips_gui_render_creature(bgui,cr,surface,
					  (creature_media_set*)ptr,
					  bgui->game->players[i]);
	}

	/*** Projectiles ***/

	for(i=0;i<bgui->game->num_projectiles;i++)
	{
		/* Find the media set which matches the object type for this object instance. */
		string_map_string_to_pointer(bgui->pr_map,bgui->game->projectiles[i]->type->pr_type_path,&ptr);

		/* render that media set according to the stat of this object instance */
		blips_gui_render_projectile(bgui,cr,surface,
					    (projectile_media_set*)ptr,
					     bgui->game->projectiles[i]);
	}

	return;
}

void blips_gui_render_breakable(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,breakable_media_set *br_set,breakable *br)
{
	cairo_surface_t *source;
	int w,h;

	/* Get the appropriate animation frame */

	if(br->time_remaining==-1)  /* is it breaking? */
	{
		source=br_set->stand_animation->frames[br->stand_animation_index];;
		br->stand_animation_index=(br->stand_animation_index+1)%(br_set->stand_animation->num_frames);
	}
	else
	{
		source=br_set->break_animation->frames[br->break_animation_index];
		br->break_animation_index=(br->break_animation_index+1)%(br_set->break_animation->num_frames);
	}

	/* for brevity */
	w=cairo_image_surface_get_width(source);
	h=cairo_image_surface_get_height(source);

	/* draw it to the appropriate place */

	cairo_translate(cr,(br->col+.5)*BLIPS_TILE_SIZE,(br->row+.5)*BLIPS_TILE_SIZE);
	cairo_set_source_surface(cr,source,-w/2.0,-h/2.0);

	cairo_paint(cr);

	cairo_identity_matrix(cr);

	return;
}

void blips_gui_render_collectible(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,collectible_media_set *co_set,collectible *co)
{
	cairo_surface_t *source;
	int w,h;

	/* Get animation frame */
	source=co_set->stand_animation->frames[co->stand_animation_index];
	co->stand_animation_index=(co->stand_animation_index+1)%(co_set->stand_animation->num_frames);

	/* for brevity */
	w=cairo_image_surface_get_width(source);
	h=cairo_image_surface_get_height(source);

	/* draw it to the appropriate place */

	cairo_translate(cr,(co->col+.5)*BLIPS_TILE_SIZE,(co->row+.5)*BLIPS_TILE_SIZE);
	cairo_set_source_surface(cr,source,-w/2.0,-h/2.0);

	cairo_paint(cr);

	cairo_identity_matrix(cr);

	return;
}

void blips_gui_render_creature(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,creature_media_set *cr_set,creature *creat)
{
	cairo_surface_t *source;
	double w,h;
	int xpos,ypos;

	/* Get the appropriate animation frame */

	if(creat->stun_count)  /* is it stunned? */
	{
		source=cr_set->stun_animation->frames[creat->stun_animation_index];
		creat->stun_animation_index=(creat->stun_animation_index+1)%(cr_set->stun_animation->num_frames);
	}
	else
		if(creat->fire_cycle_state==-1)  /* is it firing? */
			if(creat->current_move_speed)
			{
				source=cr_set->walk_animation->frames[creat->walk_animation_index];
				creat->walk_animation_index=(creat->walk_animation_index+1)%(cr_set->walk_animation->num_frames);
			}
			else
			{
				source=cr_set->stand_animation->frames[creat->stand_animation_index];
				creat->stand_animation_index=(creat->stand_animation_index+1)%(cr_set->stand_animation->num_frames);
			}
		else
			if(creat->current_move_speed)
			{
				source=cr_set->walk_and_fire_animation->frames[creat->walk_and_fire_animation_index];
				creat->walk_and_fire_animation_index=(creat->walk_and_fire_animation_index+1)%(cr_set->walk_and_fire_animation->num_frames);
			}
			else
			{
				source=cr_set->fire_animation->frames[creat->fire_animation_index];
				creat->fire_animation_index=(creat->fire_animation_index+1)%(cr_set->fire_animation->num_frames);
			}

	/* draw it to the appropriate place */

	/* for brevity */
	w=cairo_image_surface_get_width(source);
	h=cairo_image_surface_get_height(source);

	/* absolute position in pixels */
	xpos=creature_absolute_x(creat);
	ypos=creature_absolute_y(creat);

	cairo_translate(cr,xpos,ypos);
	cairo_rotate (cr,creat->aim_orientation);
	cairo_set_source_surface(cr,source,-w/2.0,-h/2.0);

	cairo_paint(cr);

	cairo_identity_matrix(cr);  /* reset transformation */

	return;
}

void blips_gui_render_projectile(blips_gui *bgui,cairo_t *cr,cairo_surface_t *surface,projectile_media_set *pr_set,projectile *pr)
{
	cairo_surface_t *source;
	double w,h;
	int xpos,ypos;

	/* Get the appropriate animation frame */

	if(pr->current_damage<0)  /* is it impacting? */
	{
		source=pr_set->impact_animation->frames[pr->impact_animation_index];
		pr->impact_animation_index=(pr->impact_animation_index+1)%(pr_set->impact_animation->num_frames);
	}
	else
	{
		source=pr_set->fly_animation->frames[pr->fly_animation_index];
		pr->fly_animation_index=(pr->fly_animation_index+1)%(pr_set->fly_animation->num_frames);
	}

	/* draw it to the appropriate place */

	/* for brevity */
	w=cairo_image_surface_get_width(source);
	h=cairo_image_surface_get_height(source);

	/* absolute position in pixels */
	xpos=pr->x_in_cell+pr->col*BLIPS_TILE_SIZE;
	ypos=pr->y_in_cell+pr->row*BLIPS_TILE_SIZE;

	cairo_translate(cr,xpos,ypos);
	cairo_rotate (cr,pr->orientation);
	cairo_set_source_surface(cr,source,-w/2.0,-h/2.0);

	cairo_paint(cr);

	cairo_identity_matrix(cr);  /* reset transformation */

	return;
}

/* events */

int blips_gui_fetch_inputs(blips_gui *bgui,SDL_Event *event,blips_input_state **inputs)
{
	/*** This function will translate SDL events into an array of blips_input_states,
	 *** which the gui will use to inform the game of events while hiding
	 *** how they are obtained. ***/
	int i;
	int deadzone;
	int joy_x,joy_y;

	deadzone=10;  /* How small joystick input has to be to be ignored */

	for(i=0;i<bgui->game->campaign->num_players;i++)
	{
		/* move and speed */
		joy_x=SDL_JoystickGetAxis(bgui->joys[i],0);
		joy_y=SDL_JoystickGetAxis(bgui->joys[i],1);

		inputs[i]->move_angle=atan2(joy_y,joy_x);
		inputs[i]->speed=sqrt(joy_x*joy_x+joy_y*joy_y)/32768.0;

		/* aim */
		joy_x=SDL_JoystickGetAxis(bgui->joys[i],3);
		joy_y=SDL_JoystickGetAxis(bgui->joys[i],2);

		if(joy_x*joy_x+joy_y*joy_y>deadzone*deadzone)
			inputs[i]->aim_angle=atan2(joy_y,joy_x);

	}

	/* modify button/joystick states as necessary */
	switch(event->type)
	{
		case SDL_QUIT:
			return 1;
		break;
		case SDL_JOYBUTTONDOWN:
			if(event->jbutton.which<bgui->game->campaign->num_players)
				inputs[event->jbutton.which]->firing=1;
		break;
		case SDL_JOYBUTTONUP:
			if(event->jbutton.which<bgui->game->campaign->num_players)
				inputs[event->jbutton.which]->firing=0;
		break;
	}

	return 0;
}

