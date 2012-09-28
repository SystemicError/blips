/* blips_game.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"blips_game.h"

blips_game* blips_game_create(void)
{
	blips_game *bgame;
	if(!(bgame=(blips_game*)malloc(sizeof(blips_game))))
	{
		fprintf(stderr,"Couldn't allocate blips_game.\n");
		exit(1);
	}

	/*** World tiles ***/

	bgame->world_tile_map=string_map_create();

	/*** Instances of user-specified types ***/

//	bgame->ais=0;
	bgame->num_ais=0;
	bgame->breakables=0;
	bgame->num_breakables=0;
	bgame->collectibles=0;
	bgame->num_collectibles=0;
	bgame->creatures=0;
	bgame->num_creatures=0;
	bgame->projectiles=0;
	bgame->num_projectiles=0;

	bgame->players=0;
	bgame->player_types=0;

	/*** User-specified types ***/

		/* keyed to file path */
	bgame->ai_types_map=string_map_create();

	bgame->pr_types_map=string_map_create();

		/* keyed to user-customized world tile symbol */

	bgame->br_types_map=string_map_create();
	bgame->co_types_map=string_map_create();
	bgame->cr_types_map=string_map_create();

	/*** Campaign ***/

	bgame->campaign=0;

	return bgame;
}

void blips_game_destroy(blips_game *bgame)
{
	int i;
	if(!bgame)
	{
		fprintf(stderr,"Attempt to free null blips_game!\n");
		exit(1);
	}

	/*** World Tiles ***/

	for(i=0;i<bgame->world_tile_map->size;i++)
		world_tile_destroy((world_tile*)(bgame->world_tile_map->pointers[i]));
	string_map_destroy(bgame->world_tile_map);

	/*** Instances of user-specified types ***/

/*	for(i=0;i<bgame->num_ais;i++)
		ai_destroy(bgame->ais[i]);
	if(bgame->num_ais)
		free(bgame->ais);
*/
	blips_game_despawn(bgame);

	for(i=0;i<bgame->campaign->num_players;i++)
	{
		creature_destroy(bgame->players[i]);
		creature_type_destroy(bgame->player_types[i]);
	}
	if(bgame->campaign->num_players)
	{
		free(bgame->players);
		free(bgame->player_types);
	}

	/*** User-specified types ***/

		/* ai_types_map strings are just pointers to the creature_type field */
	for(i=0;i<bgame->ai_types_map->size;i++)
		ai_type_destroy((ai_type*)(bgame->ai_types_map->pointers[i]));
	string_map_destroy(bgame->ai_types_map);

		/* pr_types_map strings are just pointers to a creature_type field (player or non-player) */
	for(i=0;i<bgame->pr_types_map->size;i++)
		projectile_type_destroy((projectile_type*)(bgame->pr_types_map->pointers[i]));
	string_map_destroy(bgame->pr_types_map);

	/*** Keys ***/

	for(i=0;i<bgame->br_types_map->size;i++)
	{
		breakable_type_destroy((breakable_type*)(bgame->br_types_map->pointers[i]));
		free(bgame->br_types_map->strings[i]);
	}
	string_map_destroy(bgame->br_types_map);

	for(i=0;i<bgame->co_types_map->size;i++)
	{
		collectible_type_destroy((collectible_type*)(bgame->co_types_map->pointers[i]));
		free(bgame->co_types_map->strings[i]);
	}
	string_map_destroy(bgame->co_types_map);

	for(i=0;i<bgame->cr_types_map->size;i++)
	{
		creature_type_destroy((creature_type*)(bgame->cr_types_map->pointers[i]));
		free(bgame->cr_types_map->strings[i]);
	}
	string_map_destroy(bgame->cr_types_map);

	/*** campaign ***/

	blips_campaign_destroy(bgame->campaign);

	free(bgame);
	return;
}

/* externally called requests */

void blips_game_load_campaign(blips_game *bgame,char *path)
{
	bgame->campaign=blips_campaign_create(path);
	blips_game_load_world_tiles(bgame);
	blips_game_load_players(bgame);  /* has to come before object types for projectile loading */
	blips_game_load_object_types(bgame);


	/* for first tile spawn */
	blips_game_spawn(bgame,SPAWN_ON_ENTRANCE);
	return;
}

void blips_game_step(blips_game *bgame,blips_input_state **inputs)
{
	int i;
	char *str;
	ai_type *ai_type_ptr;

int j;

	/*** Decrement creature stun counters ***/

		/* non-player */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->stun_count)
			bgame->creatures[i]->stun_count--;

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->stun_count)
			bgame->players[i]->stun_count--;

	/*** Compute AI's commands for each creature ***/

	for(i=0;i<bgame->num_creatures;i++)
	{
		str=bgame->creatures[i]->type->ai_type_path;
		string_map_string_to_pointer(bgame->ai_types_map,str,(void**)&ai_type_ptr);
		blips_game_apply_ai_type_to_creature(bgame,ai_type_ptr,bgame->creatures[i]);
	}

	/*** Use inputs as commands for player creature ***/

	for(i=0;i<bgame->campaign->num_players;i++)
	{
		bgame->players[i]->move_orientation=inputs[i]->move_angle;
		bgame->players[i]->current_move_speed=inputs[i]->speed*bgame->players[i]->type->move_speed;
		bgame->players[i]->aim_orientation=inputs[i]->aim_angle;

		/* cycle fire if firing */
		if(inputs[i]->firing)
			bgame->players[i]->fire_cycle_state=(bgame->players[i]->fire_cycle_state+1)%(bgame->players[i]->type->fire_delay);
		else
			bgame->players[i]->fire_cycle_state=-1;
	}

	/*** Move any creatures that need moving ***/

		/* non-player */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->stun_count==0)
			blips_game_move_creature(bgame,bgame->creatures[i]);

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->current_health>0)
			blips_game_move_creature(bgame,bgame->players[i]);

	/*** Spawn any necessary projectiles ***/

		/* from non-player creatures */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->fire_cycle_state==0 &&
		   bgame->creatures[i]->stun_count==0)
			blips_game_spawn_projectile_from_creature(bgame,bgame->creatures[i]);
		/* from player creatures */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->fire_cycle_state==0 && bgame->players[i]->current_health>0)
			blips_game_spawn_projectile_from_creature(bgame,bgame->players[i]);

	/*** Move any projectiles that need moving ***/

	for(i=0;i<bgame->num_projectiles;i++)
		blips_game_move_projectile(bgame,bgame->projectiles[i]);

	/*** Handle any projectile/creature, projectile/barrier, projectile/breakable collisions ***/

	for(i=0;i<bgame->num_projectiles;i++)
		if(blips_game_check_projectile_for_impact(bgame,bgame->projectiles[i]))
		{
			/* We have permission to remove this projectile */
			blips_game_remove_projectile_by_index(bgame,i);
			i--;
		}

	/*** Decrement time remaining of any breaking breakables, and remove ones that have broken */

	for(i=0;i<bgame->num_breakables;i++)
		if(bgame->breakables[i]->time_remaining>=0)
			if(bgame->breakables[i]->time_remaining==0)
			{
				/* We have permission to remove this breakable */
				breakable_destroy(bgame->breakables[i]);
				bgame->breakables[i]=bgame->breakables[bgame->num_breakables-1];
				if(!(bgame->breakables=(breakable**)realloc(bgame->breakables,sizeof(breakable*)*(bgame->num_breakables-1))) && bgame->num_breakables>1)
				{
					fprintf(stderr,"Couldn't realloc bgame->breakables.\n");
					exit(1);
				}
				if(bgame->breakables && bgame->num_breakables==0)
				{
					fprintf(stderr,"bgame->breakables is non-NULL while bgame->breakables==0.\n");
					exit(1);
				}
				bgame->num_breakables--;
				i--;
			}
			else
				bgame->breakables[i]->time_remaining--;

	/*** Remove projectiles that have left the screen entirely ***/

	blips_game_remove_projectiles_outside_boundaries(bgame);

	/*** Remove any non-player creatures who have died ***/

	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->current_health==0 &&
		   bgame->creatures[i]->stun_count==0)
		{
			creature_destroy(bgame->creatures[i]);
			bgame->creatures[i]=bgame->creatures[bgame->num_creatures-1];
			if(!(bgame->creatures=(creature**)realloc(bgame->creatures,sizeof(creature*)*(bgame->num_creatures-1))) && bgame->num_creatures>1)
			{
				fprintf(stderr,"Couldn't realloc bgame->creatures.\n");
				exit(1);
			}
			if(bgame->creatures && bgame->num_creatures==0)
			{
				fprintf(stderr,"bgame->creatures is non-NULL while bgame->creatures==0.\n");
				exit(1);
			}
			bgame->num_creatures--;
		}

	/*** Check if any player has died ***/
/*UNFINISHED*/

	/*** Check if all players are trying to leave the tile for the same direction ***/
	i=blips_game_players_leaving_world_tile(bgame);
	if(i!=-1)
		blips_game_change_active_world_tile(bgame,i);

	return;
}

world_tile* blips_game_active_world_tile(blips_game *bgame)
{
	return bgame->active_world_tile;
}

/* private functions */

void blips_game_load_world_tiles(blips_game *bgame)
{
	/***  Adds all world tiles reachable from one specified in campaign. */
	int i;
	int updated;

	world_tile *wt;

printf("Loading world tiles . . .\n");
	if(bgame->world_tile_map->size)
	{
		fprintf(stderr,"Attempt to load world tile cache into already loaded game!\n");
		exit(1);
	}

printf("Adding first world tile . . .\n");
	blips_game_add_world_tile(bgame,bgame->campaign->starting_world_tile_path);
printf("First world tile added.\n");

	/* Terribly inefficient, I know.  We only do this once.  */
	do
	{
		updated=0;
		for(i=0;i<bgame->world_tile_map->size;i++)
		{
			wt=(world_tile*)(bgame->world_tile_map->pointers[i]);
			if((strcmp("none",wt->north_tile) && blips_game_add_world_tile(bgame,wt->north_tile)) ||
			   (strcmp("none",wt->east_tile) && blips_game_add_world_tile(bgame,wt->east_tile)) ||
			   (strcmp("none",wt->south_tile) && blips_game_add_world_tile(bgame,wt->south_tile)) ||
			   (strcmp("none",wt->west_tile) && blips_game_add_world_tile(bgame,wt->west_tile)))
			updated=1;
		}
	}while(updated);

	string_map_string_to_pointer(bgame->world_tile_map,bgame->campaign->starting_world_tile_path,(void**)&(bgame->active_world_tile));

	return;
}

int blips_game_add_world_tile(blips_game *bgame,char *path)
{
	/*** Adds a single world tile to the world_tiles array.  Returns
	 *** non-zero if added, zero if a rejected duplicate.  */

	int i;
	void *ptr;

	/* scan for duplication */
	string_map_string_to_pointer(bgame->world_tile_map,path,&ptr);

	if(ptr && bgame->world_tile_map->size)
{
printf("Rejecting %s; duplicate.\n",path);
		return 0;
}

	string_map_add(bgame->world_tile_map,path,(void*)world_tile_create(path));
	return 1;
}

	/* clear objects from game */
void blips_game_despawn(blips_game *bgame)
{
	int i;

	for(i=0;i<bgame->num_breakables;i++)
		breakable_destroy(bgame->breakables[i]);
	if(bgame->num_breakables)
		free(bgame->breakables);
	bgame->breakables=0;
	bgame->num_breakables=0;

	for(i=0;i<bgame->num_collectibles;i++)
		collectible_destroy(bgame->collectibles[i]);
	if(bgame->num_collectibles)
		free(bgame->collectibles);
	bgame->collectibles=0;
	bgame->num_collectibles=0;

	for(i=0;i<bgame->num_creatures;i++)
		creature_destroy(bgame->creatures[i]);
	if(bgame->num_creatures)
		free(bgame->creatures);
	bgame->creatures=0;
	bgame->num_creatures=0;

	for(i=0;i<bgame->num_projectiles;i++)
		projectile_destroy(bgame->projectiles[i]);
	if(bgame->num_projectiles)
		free(bgame->projectiles);
	bgame->projectiles=0;
	bgame->num_projectiles=0;

	return;
}

	/* spawn objects which follow spawn_trigger trigger from active tile */
void blips_game_spawn(blips_game *bgame,spawn_trigger trigger)
{
	int i,j;
	char string[3];

	breakable_type *br_type;
	collectible_type *co_type;
	creature_type *cr_type;

	string[2]=0;

	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			/* Fetch a string from the world tile  */

			string[0]=bgame->active_world_tile->object_strings[i][j][0];
			string[1]=bgame->active_world_tile->object_strings[i][j][1];

			/* Compare the string to our object key
			 * and add an object of the type specified */

				/* breakable types */
			string_map_string_to_pointer(bgame->br_types_map,string,(void**)&br_type);

			if(br_type && br_type->trigger==trigger)
			{
				if(!(bgame->breakables=(breakable**)realloc(bgame->breakables,sizeof(breakable*)*(bgame->num_breakables+1))))
				{
					fprintf(stderr,"Couldn't realloc bgame->breakables.\n");
					exit(1);
				}
				bgame->breakables[bgame->num_breakables]=breakable_create(br_type);

				bgame->breakables[bgame->num_breakables]->row=i;
				bgame->breakables[bgame->num_breakables]->col=j;

				bgame->num_breakables++;
			}

				/* search collectible types */
			string_map_string_to_pointer(bgame->co_types_map,string,(void**)&co_type);

			if(co_type && co_type->trigger==trigger)
			{
				if(!(bgame->collectibles=(collectible**)realloc(bgame->collectibles,sizeof(collectible*)*(bgame->num_collectibles+1))))
				{
					fprintf(stderr,"Couldn't realloc bgame->collectibles.\n");
					exit(1);
				}
				bgame->collectibles[bgame->num_collectibles]=collectible_create(co_type);

				bgame->collectibles[bgame->num_collectibles]->row=i;
				bgame->collectibles[bgame->num_collectibles]->col=j;

				bgame->num_collectibles++;
			}

				/* search creature types */
			string_map_string_to_pointer(bgame->cr_types_map,string,(void**)&cr_type);

			if(cr_type && cr_type->trigger==trigger)
			{
				if(!(bgame->creatures=(creature**)realloc(bgame->creatures,sizeof(creature*)*(bgame->num_creatures+1))))
				{
					fprintf(stderr,"Couldn't realloc bgame->creatures.\n");
					exit(1);
				}
				bgame->creatures[bgame->num_creatures]=creature_create(cr_type);

				bgame->creatures[bgame->num_creatures]->row=i;
				bgame->creatures[bgame->num_creatures]->col=j;
				bgame->creatures[bgame->num_creatures]->team=1;

				bgame->num_creatures++;
			}
		}

	return;
}

int blips_game_players_leaving_world_tile(blips_game *bgame)
{
	/*** This function checks if all players are trying to leave the tile
	 *** at a location where no barrier blocks them and in a direction
	 *** that leads to another tile. */
	int i;
	int north,east,south,west;
	int direction;
	double tolerance;

	/* how small can the dot product of the orientation and coordinate
	 * vectors be while still indicating a desire to change tile? */
	tolerance=.9;

	north=east=south=west=1;

	/* First, let's make sure that everyone is at the edge of the screen. */
	/* Also eliminate candidate directions everyone could be going. */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->fire_cycle_state!=-1 ||
		   (bgame->players[i]->row>0 && bgame->players[i]->row<BLIPS_TILE_ROWS-1 &&
		    bgame->players[i]->col>0 && bgame->players[i]->col<BLIPS_TILE_COLS-1))
			return -1;
		else
		{
			/* record what side they're on and their orientation */
			if(bgame->players[i]->row!=0 ||
			   -sin(bgame->players[i]->aim_orientation)<tolerance)
				north=0;

			if(bgame->players[i]->col!=BLIPS_TILE_COLS-1 ||
			   cos(bgame->players[i]->aim_orientation)<tolerance)
				east=0;

			if(bgame->players[i]->row!=BLIPS_TILE_ROWS-1 ||
			   sin(bgame->players[i]->aim_orientation)<tolerance)
				south=0;

			if(bgame->players[i]->col!=0 ||
			   -cos(bgame->players[i]->aim_orientation)<tolerance)
				west=0;
		}

	/* If no one edge is agreed upon, return no direction */
	if(!(north || east || south || west))
		return -1;

	/* Okay, now everyone is facing the right direction, in the right cell,
	 * and holding fire.  But is there a barrier in the way? */

	if(north)
		direction=MAZE_NORTH;
	if(east)
		direction=MAZE_EAST;
	if(south)
		direction=MAZE_SOUTH;
	if(west)
		direction=MAZE_WEST;

	for(i=0;i<bgame->campaign->num_players;i++)
		if(maze_contains_wall(bgame->active_world_tile->creature_barriers,
				      bgame->players[i]->row,
				      bgame->players[i]->col,
				      direction))
			return -1;

	/* Make sure there actually is a world tile in that direction */

	if(!strcmp(bgame->active_world_tile->north_tile,"none") && north)
		return -1;
	if(!strcmp(bgame->active_world_tile->east_tile,"none") && east)
		return -1;
	if(!strcmp(bgame->active_world_tile->south_tile,"none") && south)
		return -1;
	if(!strcmp(bgame->active_world_tile->west_tile,"none") && west)
		return -1;

	/* Finally, check if they're actually bumping up against the edge. */

	for(i=0;i<bgame->campaign->num_players;i++)
		if(!blips_game_move_creature(bgame,bgame->players[i]))
			return -1;

printf("Agreed on direction %d.\n",direction);
	return direction;
}

void blips_game_change_active_world_tile(blips_game *bgame,int direction)
{
	/*** This function is called when the game has detected that all players
	 *** are leaving one tile for the next in the direction given.
	 *** It must remove current objects and add the correct new ones. */

	int i;

	/* get rid of old objects */
	blips_game_despawn(bgame);

	/* place players in the correct position on new screen */
	for(i=0;i<bgame->campaign->num_players;i++)
		switch(direction)
		{
			case MAZE_NORTH:
				bgame->players[i]->row=BLIPS_TILE_ROWS-1;
				bgame->players[i]->y_in_cell=BLIPS_TILE_SIZE-bgame->players[i]->y_in_cell;
			break;
			case MAZE_EAST:
				bgame->players[i]->col=0;
				bgame->players[i]->x_in_cell=BLIPS_TILE_SIZE-bgame->players[i]->x_in_cell;
			break;
			case MAZE_SOUTH:
				bgame->players[i]->row=0;
				bgame->players[i]->y_in_cell=BLIPS_TILE_SIZE-bgame->players[i]->y_in_cell;
			break;
			case MAZE_WEST:
				bgame->players[i]->col=BLIPS_TILE_COLS-1;
				bgame->players[i]->x_in_cell=BLIPS_TILE_SIZE-bgame->players[i]->x_in_cell;
			break;
		}

	/* change active world tile */
printf("East path is: %s.\n",bgame->active_world_tile->east_tile);
printf("Tile Map dump:\n");
for(i=0;i<bgame->world_tile_map->size;i++)
printf("path:  %s -- %d.\n",((world_tile*)(bgame->world_tile_map->pointers[i]))->path,bgame->world_tile_map->pointers[i]);
	switch(direction)
	{
		case MAZE_NORTH:
			string_map_string_to_pointer(bgame->world_tile_map,bgame->active_world_tile->north_tile,(void**)&(bgame->active_world_tile));
		break;
		case MAZE_EAST:
			string_map_string_to_pointer(bgame->world_tile_map,bgame->active_world_tile->east_tile,(void**)&(bgame->active_world_tile));
		break;
		case MAZE_SOUTH:
			string_map_string_to_pointer(bgame->world_tile_map,bgame->active_world_tile->south_tile,(void**)&(bgame->active_world_tile));
		break;
		case MAZE_WEST:
			string_map_string_to_pointer(bgame->world_tile_map,bgame->active_world_tile->west_tile,(void**)&(bgame->active_world_tile));
		break;
	}

	if(!(bgame->active_world_tile))
	{
		fprintf(stderr,"Couldn't find world tile of correct path!.\n");
		exit(1);
	}

	/* add new objects */
	blips_game_spawn(bgame,SPAWN_ON_ENTRANCE);

	return;
}

void blips_game_load_object_types(blips_game *bgame)
{
	FILE *fp;
	int i,count;
	char *string;
	char buffer[BUFFER_SIZE];
	ai_type *ai_type_ptr;
	projectile_type *pr_type;

	if(!(fp=fopen(bgame->campaign->object_key_path,"r")))
	{
		fprintf(stderr,"Couldn't open object_key path:  %s\n.",bgame->campaign->object_key_path);
		exit(1);
	}


	/*** Breakable Types/Strings ***/
printf("Loading breakable types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Error parsing number of breakable strings in object_key:  %s.\n",bgame->campaign->object_key_path);
		exit(1);
	}
printf("Loading %d breakable types.\n",count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		if(!(string=(char*)malloc(sizeof(char)*3)))
		{
			fprintf(stderr,"Couldn't alloc br string in bgame_load_object_types().\n");
			exit(1);
		}
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fgetc(fp);  /* the '=' symbol */
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		string_map_add(bgame->br_types_map,
				string,
				(void*)breakable_type_create(buffer));
	}

	/*** Collectible Types/Strings ***/
printf("Loading collectible types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Error parsing number of collectible strings in object_key:  %s.\n",bgame->campaign->object_key_path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		if(!(string=(char*)malloc(sizeof(char)*3)))
		{
			fprintf(stderr,"Couldn't alloc co string in bgame_load_object_types().\n");
			exit(1);
		}
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fgetc(fp);  /* the '=' symbol */
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		string_map_add(bgame->co_types_map,
				string,
				(void*)collectible_type_create(buffer));
	}

	/*** Creature Types/Strings ***/
printf("Loading creature types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Error parsing number of creature strings in object_key:  %s.\n",bgame->campaign->object_key_path);
		exit(1);
	}
printf("Reading %d creatures.\n",count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		if(!(string=(char*)malloc(sizeof(char)*3)))
		{
			fprintf(stderr,"Couldn't alloc cr string in bgame_load_object_types().\n");
			exit(1);
		}
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fgetc(fp);  /* the '=' symbol */
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		string_map_add(bgame->cr_types_map,
				string,
				(void*)creature_type_create(buffer));
	}

	fclose(fp);

	/*** AI Types ***/

	/* These have to be loaded from creature types list. */

printf("Loading ai_types . . .\n");
	for(i=0;i<bgame->cr_types_map->size;i++)
	{
		string=((creature_type*)(bgame->cr_types_map->pointers[i]))->ai_type_path;
		ai_type_ptr=ai_type_create(string);
		string_map_add(bgame->ai_types_map,string,ai_type_ptr);
	}

	/*** Projectile Types ***/
printf("Loading projectiles types . . .\n");

	/* These have to be loaded from creature AND player list, checked for duplicates. */

		/* non-player */
	for(i=0;i<bgame->cr_types_map->size;i++)
	{
		/* Get pr_type string from non-player creature array */
		string=((creature_type*)(bgame->cr_types_map->pointers[i]))->pr_type_path;

		/* See if we already have one of this type */
		string_map_string_to_pointer(bgame->pr_types_map,string,(void**)&pr_type);

		if(!pr_type)  /* we don't, so add it */
		{
			pr_type=projectile_type_create(string);
			string_map_add(bgame->pr_types_map,string,pr_type);
		}
	}

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
	{
		/* Get pr_type string from non-player creature array */
		string=bgame->player_types[i]->pr_type_path;

		/* See if we already have one of this type */
		string_map_string_to_pointer(bgame->pr_types_map,string,(void**)&pr_type);

		if(!pr_type)  /* we don't, so add it */
		{
			pr_type=projectile_type_create(string);
			string_map_add(bgame->pr_types_map,string,pr_type);
		}
	}

	return;
}

void blips_game_load_players(blips_game *bgame)
{
	int i;

	if(!(bgame->players=(creature**)malloc(sizeof(creature*)*bgame->campaign->num_players)))
	{
		fprintf(stderr,"Couldn't alloc bgame->players.\n");
		exit(1);
	}
	if(!(bgame->player_types=(creature_type**)malloc(sizeof(creature_type*)*bgame->campaign->num_players)))
	{
		fprintf(stderr,"Couldn't alloc bgame->player_types.\n");
		exit(1);
	}

	for(i=0;i<bgame->campaign->num_players;i++)
	{
		bgame->player_types[i]=creature_type_create(bgame->campaign->player_type_file_paths[i]);
		bgame->players[i]=creature_create(bgame->player_types[i]);

		bgame->players[i]->row=bgame->campaign->player_starting_rows[i];
		bgame->players[i]->col=bgame->campaign->player_starting_cols[i];
		bgame->players[i]->team=0;
	}

	return;
}

void blips_game_remove_projectile_by_index(blips_game *bgame,int i)
{
	projectile_destroy(bgame->projectiles[i]);
	bgame->projectiles[i]=bgame->projectiles[bgame->num_projectiles-1];
	if(!(bgame->projectiles=(projectile**)realloc(bgame->projectiles,sizeof(projectile*)*(bgame->num_projectiles-1))) && bgame->num_projectiles>1)
	{
		fprintf(stderr,"Couldn't realloc bgame->projectiles.\n");
		exit(1);
	}
	if(bgame->projectiles && bgame->num_projectiles==0)
	{
		fprintf(stderr,"bgame->projectiles is non-NULL while bgame->projectiles==0.\n");
		exit(1);
	}
	bgame->num_projectiles--;

	return;
}

void blips_game_apply_ai_type_to_creature(blips_game *bgame,ai_type *ai_type_ptr,creature *cr)
{
	creature *enemy_cr;
	projectile *enemy_pr;


	/* get nearest enemy creature */

	enemy_cr=ai_directives_get_nearest_enemy_creature(bgame,cr);

	/* get nearest enemy projectile */

	enemy_pr=ai_directives_get_nearest_enemy_projectile(bgame,cr);

	/* move goal affects creature's facing direction and speed */

	switch(ai_type_ptr->move_goal)
	{
		case AI_DODGE:
			ai_directives_move_dodge(bgame,cr,enemy_pr);
		break;
		case AI_FLEE:
			ai_directives_move_flee(bgame,cr,enemy_cr);
		break;
		case AI_CLOSE:
			ai_directives_move_close(bgame,cr,enemy_cr);
		break;
		case AI_WANDER:
			ai_directives_move_wander(bgame,cr);
		break;
		case AI_PATROL_NS:
			ai_directives_move_patrol_ns(bgame,cr);
		break;
		case AI_PATROL_EW:
			ai_directives_move_patrol_ew(bgame,cr);
		break;
	}

	/* aim goal affects creature's aim */

	if(enemy_cr)
		switch(ai_type_ptr->aim_goal)
		{
			case AI_FACE:
				ai_directives_aim_face(bgame,cr,enemy_cr);
			break;
			case AI_SPIN:
				ai_directives_aim_spin(bgame,cr);
			break;
			case AI_LAG_FACE:
				ai_directives_aim_lag_face(bgame,cr,enemy_cr);
			break;
			case AI_LEAD_FACE:
				ai_directives_aim_lead_face(bgame,cr,enemy_cr);
			break;
			case AI_AS_VELOCITY:
				ai_directives_aim_as_velocity(bgame,cr);
			break;
		}

	/* fire goal affects creature's fire_cycle_state */

	if(enemy_cr)
		switch(ai_type_ptr->fire_goal)
		{
			case AI_SPAM:
				ai_directives_fire_spam(bgame,cr);
			break;
			case AI_SIGHT_FIRST:
				ai_directives_fire_sight_first(bgame,cr,enemy_cr);
			break;
			case AI_SPURT:
				ai_directives_fire_spurt(bgame,cr,enemy_cr);
			break;
		}
	else
		cr->fire_cycle_state=-1;

	return;
}

int blips_game_move_creature(blips_game *bgame,creature *cr)
{
	/*** Moves creature according to its orientation, speed.  If
	 *** doing so would cause a collision, it undoes it and returns
	 *** non-zero. */

	double delta_x,delta_y;
	int i;
	int old_row,old_col;
	double old_x,old_y;

	/* in case we have to reverse changes later */

	old_col=cr->col;
	old_row=cr->row;
	old_x=cr->x_in_cell;
	old_y=cr->y_in_cell;

	/* calculate change we're going to make */

	delta_x=cr->current_move_speed*cos(cr->move_orientation);
	delta_y=cr->current_move_speed*sin(cr->move_orientation);

	cr->x_in_cell+=delta_x;
	cr->y_in_cell+=delta_y;

	/* change cells */

	if(cr->x_in_cell<0)
	{
		cr->x_in_cell+=BLIPS_TILE_SIZE;
		cr->col--;
	}
	if(cr->x_in_cell>=BLIPS_TILE_SIZE)
	{
		cr->x_in_cell-=BLIPS_TILE_SIZE;
		cr->col++;
	}
	if(cr->y_in_cell<0)
	{
		cr->y_in_cell+=BLIPS_TILE_SIZE;
		cr->row--;
	}
	if(cr->y_in_cell>=BLIPS_TILE_SIZE)
	{
		cr->y_in_cell-=BLIPS_TILE_SIZE;
		cr->row++;
	}

	/* check if movement causes a collision.  If it does,
	 * reverse this movement. */

	if(blips_game_creature_intersects_creatures(bgame,cr) ||
	   blips_game_creature_intersects_boundaries(bgame,cr) ||
	   blips_game_creature_intersects_barriers(bgame,cr) ||
	   blips_game_creature_intersects_breakables(bgame,cr))
	{
		/* reverse changes */
		cr->col=old_col;
		cr->row=old_row;
		cr->x_in_cell=old_x;
		cr->y_in_cell=old_y;
		return 1;
	}

	return 0;
}

void blips_game_move_projectile(blips_game *bgame,projectile *pr)
{
	int i;
	double delta_x,delta_y;

	if(pr->current_damage<=0)  /* don't move those that are already impacted */
		return;

	delta_x=pr->type->move_speed*cos(pr->orientation);
	delta_y=pr->type->move_speed*sin(pr->orientation);

	pr->x_in_cell+=delta_x;
	pr->y_in_cell+=delta_y;

	/* change cells */

	if(pr->x_in_cell<0)
	{
		pr->x_in_cell+=BLIPS_TILE_SIZE;
		pr->col--;
	}
	if(pr->x_in_cell>=BLIPS_TILE_SIZE)
	{
		pr->x_in_cell-=BLIPS_TILE_SIZE;
		pr->col++;
	}
	if(pr->y_in_cell<0)
	{
		pr->y_in_cell+=BLIPS_TILE_SIZE;
		pr->row--;
	}
	if(pr->y_in_cell>=BLIPS_TILE_SIZE)
	{
		pr->y_in_cell-=BLIPS_TILE_SIZE;
		pr->row++;
	}

	return;
}

void blips_game_spawn_projectile_from_creature(blips_game *bgame,creature *cr)
{
	projectile *pr;
	projectile_type *pr_type;

	/* Get the type of projectile this creature creates */
	string_map_string_to_pointer(bgame->pr_types_map,cr->type->pr_type_path,(void**)&pr_type);

	/* make sure its projectile type exists */
	if(!pr_type)
	{
		fprintf(stderr,"Couldn't find projectile type of firing creature in pr_types_map!\n");
		exit(1);
	}

	/* create an instance of it */
	pr=projectile_create(pr_type);

	/* place and orient it according to creature */
	pr->row=cr->row;
	pr->col=cr->col;
	pr->x_in_cell=cr->x_in_cell;
	pr->y_in_cell=cr->y_in_cell;
	pr->orientation=cr->aim_orientation;
	pr->team=cr->team;

	/* add it to the projectile list */
	if(!(bgame->projectiles=(projectile**)realloc(bgame->projectiles,sizeof(projectile*)*(bgame->num_projectiles+1))))
	{
		fprintf(stderr,"Couldn't realloc bgame->projectiles.\n");
		exit(1);
	}
	bgame->projectiles[bgame->num_projectiles]=pr;
	bgame->num_projectiles++;

	return;
}

void blips_game_remove_projectiles_outside_boundaries(blips_game *bgame)
{
	/*** This will remove projectiles that have thoroughly left the screen */

	int i;
	for(i=0;i<bgame->num_projectiles;i++)
		if(bgame->projectiles[i]->col<-1 ||
		   bgame->projectiles[i]->col>=BLIPS_TILE_COLS+1 ||
		   bgame->projectiles[i]->row<-1 ||
		   bgame->projectiles[i]->row>=BLIPS_TILE_ROWS+1)
		{
			blips_game_remove_projectile_by_index(bgame,i);
			i--;
		}

	return;
}

int blips_game_creature_intersects_creatures(blips_game *bgame,creature *cr)
{
	int i;
	double distance_squared;
	double margin;

	margin=BLIPS_TILE_SIZE/2.0;

		/* non-player */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]!=cr)  /* don't check creature against itself */
		{
			distance_squared=pow(creature_absolute_x(bgame->creatures[i])-creature_absolute_x(cr),2)+
					 pow(creature_absolute_y(bgame->creatures[i])-creature_absolute_y(cr),2);
			if(distance_squared<margin*margin)
				return 1;
		}

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]!=cr)  /* don't check creature against itself */
		{
			distance_squared=pow(creature_absolute_x(bgame->players[i])-creature_absolute_x(cr),2)+
					 pow(creature_absolute_y(bgame->players[i])-creature_absolute_y(cr),2);
			if(distance_squared<margin*margin)
				return 1;
		}

	return 0;
}

int blips_game_creature_intersects_boundaries(blips_game *bgame,creature *cr)
{
	if((cr->col==0 && cr->x_in_cell<BLIPS_TILE_SIZE/3.0) ||
	   (cr->col==BLIPS_TILE_COLS-1 && cr->x_in_cell>BLIPS_TILE_SIZE*2.0/3.0) ||
	   (cr->row==0 && cr->y_in_cell<BLIPS_TILE_SIZE/3.0) ||
	   (cr->row==BLIPS_TILE_ROWS-1 && cr->y_in_cell>BLIPS_TILE_SIZE*2.0/3.0))
		return 1;

	if(cr->col<0 ||
	   cr->col>=BLIPS_TILE_COLS ||
	   cr->row<0 ||
	   cr->row>=BLIPS_TILE_ROWS)
	{
		fprintf(stderr,"Creature is *far* outside boundaries!\n");
		return 1;
	}

	return 0;
}

int blips_game_creature_intersects_barriers(blips_game *bgame,creature *cr)
{
	double margin;

	margin=BLIPS_TILE_SIZE/4.0;

	if((maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col,MAZE_NORTH) &&
	    cr->y_in_cell<margin) ||
	   (maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col,MAZE_EAST) &&
	    cr->x_in_cell>BLIPS_TILE_SIZE-margin) ||
	   (maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col,MAZE_SOUTH) &&
	    cr->y_in_cell>BLIPS_TILE_SIZE-margin) ||
	   (maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col,MAZE_WEST) &&
	    cr->x_in_cell<margin))
		return 1;

	/* corners */

		/* northeast */
	if(maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col+1,MAZE_NORTH) &&
	   maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row-1,cr->col,MAZE_EAST) &&
	   pow(BLIPS_TILE_SIZE-cr->x_in_cell,2)+pow(cr->y_in_cell,2)<margin*margin)
		return 1;

		/* southeast */
	if(maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col+1,MAZE_SOUTH) &&
	   maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row+1,cr->col,MAZE_EAST) &&
	   pow(BLIPS_TILE_SIZE-cr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-cr->y_in_cell,2)<margin*margin)
		return 1;

		/* southwest */
	if(maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col-1,MAZE_SOUTH) &&
	   maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row+1,cr->col,MAZE_WEST) &&
	   pow(cr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-cr->y_in_cell,2)<margin*margin)
		return 1;

		/* northwest */
	if(maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row,cr->col-1,MAZE_NORTH) &&
	   maze_contains_wall(bgame->active_world_tile->creature_barriers,cr->row-1,cr->col,MAZE_WEST) &&
	   pow(cr->x_in_cell,2)+pow(cr->y_in_cell,2)<margin*margin)
		return 1;

	return 0;
}

int blips_game_creature_intersects_breakables(blips_game *bgame,creature *cr)
{
	int i;
	double margin;

	margin=BLIPS_TILE_SIZE/4.0;

	for(i=0;i<bgame->num_breakables;i++)
	{
		if((bgame->breakables[i]->col==cr->col && bgame->breakables[i]->row==cr->row-1 &&
		    cr->y_in_cell<margin) ||
		   (bgame->breakables[i]->col==cr->col+1 && bgame->breakables[i]->row==cr->row &&
		    cr->x_in_cell>BLIPS_TILE_SIZE-margin) ||
		   (bgame->breakables[i]->col==cr->col && bgame->breakables[i]->row==cr->row+1 &&
		    cr->y_in_cell>BLIPS_TILE_SIZE-margin) ||
		   (bgame->breakables[i]->col==cr->col-1 && bgame->breakables[i]->row==cr->row &&
		    cr->x_in_cell<margin))
			return 1;

		/* corners */

				/* northeast */
		if(bgame->breakables[i]->col==cr->col+1 && bgame->breakables[i]->row==cr->row-1 &&
		   pow(BLIPS_TILE_SIZE-cr->x_in_cell,2)+pow(cr->y_in_cell,2)<margin*margin)
			return 1;

				/* southeast */
		if(bgame->breakables[i]->col==cr->col+1 && bgame->breakables[i]->row==cr->row+1 &&
		   pow(BLIPS_TILE_SIZE-cr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-cr->y_in_cell,2)<margin*margin)
			return 1;

				/* southwest */
		if(bgame->breakables[i]->col==cr->col-1 && bgame->breakables[i]->row==cr->row+1 &&
		   pow(cr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-cr->y_in_cell,2)<margin*margin)
			return 1;

				/* northwest */
		if(bgame->breakables[i]->col==cr->col-1 && bgame->breakables[i]->row==cr->row-1 &&
		   pow(cr->x_in_cell,2)+pow(cr->y_in_cell,2)<margin*margin)
			return 1;
	}
	return 0;
}

int blips_game_check_projectile_for_impact(blips_game *bgame,projectile *pr)
{
	/* Returns nonzero iff this projectile has already impacted and is due for removal */
	int i;

	if(pr->current_damage<0)  /* if it's already awaiting removal */
	{
		pr->current_damage++;
		return 0;
	}
	if(pr->current_damage>0)  /* if it's still flying */
	{
		/* check to see if it intersects any barriers; if it has, impact it */

		if(blips_game_projectile_intersects_barriers(bgame,pr))
		{
			pr->current_damage=-pr->type->despawn_delay;
			return 0;
		}

		/* check to see if it intersects any breakables; if it has,
		 * have them react appropriately and reduce the damage of this projectile */

		for(i=0;i<bgame->num_breakables;i++)
			if(pr->col==bgame->breakables[i]->col && pr->row==bgame->breakables[i]->row && bgame->breakables[i]->time_remaining==-1)  /*TEMPORARY*/
			{
				if(bgame->breakables[i]->type->toughness<=pr->current_damage)
					bgame->breakables[i]->time_remaining=bgame->breakables[i]->type->despawn_delay;

				/* decrement the pr's current damage */
				pr->current_damage-=bgame->breakables[i]->type->toughness;
			}

		/* If the projectile now has damage<=0, impact it */

		if(pr->current_damage<=0)
		{
			pr->current_damage=-pr->type->despawn_delay;
			return 0;
		}

		/* check to see if pr intersect creatures and have them react
		 * appropriately, i.e. set stun timer and take damage */

			/* non-player */
		for(i=0;i<bgame->num_creatures;i++)
			if(bgame->creatures[i]->team!=pr->team && bgame->creatures[i]->current_health>0)
				/* The following line is TEMPORARY collision detection */
				if(pow(creature_absolute_x(bgame->creatures[i])-projectile_absolute_x(pr),2)+
				   pow(creature_absolute_y(bgame->creatures[i])-projectile_absolute_y(pr),2)<
				   BLIPS_TILE_SIZE*BLIPS_TILE_SIZE/5.0)
				{
					bgame->creatures[i]->stun_count=bgame->creatures[i]->type->stun_delay;
					if(bgame->creatures[i]->current_health<=pr->current_damage)
					{
						pr->current_damage-=bgame->creatures[i]->current_health;
						bgame->creatures[i]->current_health=0;
						if(pr->current_damage<=0)
							pr->current_damage=-pr->type->despawn_delay;
					}
					else
					{
						bgame->creatures[i]->current_health-=pr->current_damage;
						pr->current_damage=-pr->type->despawn_delay;
					}
					return 0;
				}

			/* player */
		for(i=0;i<bgame->campaign->num_players;i++)
			if(bgame->players[i]->team!=pr->team && bgame->players[i]->current_health>0)
				/* The following line is TEMPORARY collision detection */
				if(pow(creature_absolute_x(bgame->players[i])-projectile_absolute_x(pr),2)+
				   pow(creature_absolute_y(bgame->players[i])-projectile_absolute_y(pr),2)<
				   BLIPS_TILE_SIZE*BLIPS_TILE_SIZE/5.0)
				{
					bgame->players[i]->stun_count=bgame->players[i]->type->stun_delay;
					if(bgame->players[i]->current_health<=pr->current_damage)
					{
						pr->current_damage-=bgame->players[i]->current_health;
						bgame->players[i]->current_health=0;
						if(pr->current_damage<=0)
							pr->current_damage=-pr->type->despawn_delay;
					}
					else
					{
						bgame->players[i]->current_health-=pr->current_damage;
						pr->current_damage=-pr->type->despawn_delay;
					}
					return 0;
				}


		return 0;
	}

	/* it's at current_damage==0, which means it's time to get rid of it */
	return 1;
}

int blips_game_projectile_intersects_barriers(blips_game *bgame,projectile *pr)
{
	double margin;

	margin=.1*BLIPS_TILE_SIZE;

	if((maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col,MAZE_NORTH) &&
	    pr->y_in_cell<margin) ||
	   (maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col,MAZE_EAST) &&
	    pr->x_in_cell>BLIPS_TILE_SIZE-margin) ||
	   (maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col,MAZE_SOUTH) &&
	    pr->y_in_cell>BLIPS_TILE_SIZE-margin) ||
	   (maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col,MAZE_WEST) &&
	    pr->x_in_cell<margin))
		return 1;

	/* corners */

		/* northeast */
	if(maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col+1,MAZE_NORTH) &&
	   maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row-1,pr->col,MAZE_EAST) &&
	   pow(BLIPS_TILE_SIZE-pr->x_in_cell,2)+pow(pr->y_in_cell,2)<margin*margin)
		return 1;

		/* southeast */
	if(maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col+1,MAZE_SOUTH) &&
	   maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row+1,pr->col,MAZE_EAST) &&
	   pow(BLIPS_TILE_SIZE-pr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-pr->y_in_cell,2)<margin*margin)
		return 1;

		/* southwest */
	if(maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col-1,MAZE_SOUTH) &&
	   maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row+1,pr->col,MAZE_WEST) &&
	   pow(pr->x_in_cell,2)+pow(BLIPS_TILE_SIZE-pr->y_in_cell,2)<margin*margin)
		return 1;

		/* northeast */
	if(maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row,pr->col-1,MAZE_NORTH) &&
	   maze_contains_wall(bgame->active_world_tile->projectile_barriers,pr->row-1,pr->col,MAZE_WEST) &&
	   pow(pr->x_in_cell,2)+pow(pr->y_in_cell,2)<margin*margin)
		return 1;

	return 0;
}

creature* ai_directives_get_nearest_enemy_creature(blips_game *bgame,creature *cr)
{
	creature *enemy_cr;
	int i;
	double cr_abs_x,cr_abs_y;
	double distance_squared,candidate_distance_squared;
	double candidate_x,candidate_y;

	/* compute this creatures absolute position */

	cr_abs_x=creature_absolute_x(cr);
	cr_abs_y=creature_absolute_y(cr);

	/* get nearest enemy creature */

	distance_squared=-1;

		/* search non-player creatures */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->team!=cr->team && bgame->creatures[i]->current_health>0)
		{
			candidate_x=creature_absolute_x(bgame->creatures[i]);
			candidate_y=creature_absolute_y(bgame->creatures[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_cr=bgame->creatures[i];
				distance_squared=candidate_distance_squared;
			}
		}

		/* search player creatures */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->team!=cr->team && bgame->players[i]->current_health>0)
		{
			candidate_x=creature_absolute_x(bgame->players[i]);
			candidate_y=creature_absolute_y(bgame->players[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_cr=bgame->players[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==-1)
		return 0;
	return enemy_cr;
}

projectile* ai_directives_get_nearest_enemy_projectile(blips_game *bgame,creature *cr)
{
	double distance_squared,candidate_distance_squared;
	projectile *enemy_pr;
	double cr_abs_x,cr_abs_y;
	double candidate_x,candidate_y;
	int i;

	/* compute this creatures absolute position */

	cr_abs_x=creature_absolute_x(cr);
	cr_abs_y=creature_absolute_y(cr);

	/* get nearest enemy projectile */

	distance_squared=-1;

	for(i=0;i<bgame->num_projectiles;i++)
		if(bgame->projectiles[i]->team!=cr->team)
		{
			candidate_x=projectile_absolute_x(bgame->projectiles[i]);
			candidate_y=projectile_absolute_y(bgame->projectiles[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_pr=bgame->projectiles[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==-1)
		return 0;
	return enemy_pr;
}

void ai_directives_move_dodge(blips_game *bgame,creature *cr,projectile *enemy_pr)
{
	if(enemy_pr)
	{
		cr->current_move_speed=cr->type->move_speed;

		/* if the cross product of the pr's vel and
		 * the vector from pr to cr has positive z,
		 * then we should SUBTRACT M_PI/2 from orientation */

		/* compute cross product's z */
		if(cos(enemy_pr->orientation)*(projectile_absolute_y(enemy_pr)-creature_absolute_y(cr))-
		   sin(enemy_pr->orientation)*(projectile_absolute_x(enemy_pr)-creature_absolute_x(cr))<0)
			cr->move_orientation=enemy_pr->orientation+M_PI/2.0;
		else
			cr->move_orientation=enemy_pr->orientation-M_PI/2.0;
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_flee(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	if(enemy_cr)
	{
		cr->current_move_speed=cr->type->move_speed;
		cr->move_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
					   creature_absolute_x(enemy_cr)-creature_absolute_x(cr))+
				     M_PI;
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_close(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	if(enemy_cr)
	{
		cr->current_move_speed=cr->type->move_speed;
		cr->move_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
					   creature_absolute_x(enemy_cr)-creature_absolute_x(cr));
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_wander(blips_game *bgame,creature *cr)
{
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation=rand();
	return;
}

void ai_directives_move_patrol_ns(blips_game *bgame,creature *cr)
{
	if(abs(cos(cr->move_orientation))>.01)
		cr->move_orientation=M_PI/2.0;
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation+=M_PI;
	return;
}

void ai_directives_move_patrol_ew(blips_game *bgame,creature *cr)
{
	if(abs(sin(cr->move_orientation))>.01)
		cr->move_orientation=M_PI;
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation+=M_PI;
	return;
}

void ai_directives_aim_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	cr->aim_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
				  creature_absolute_x(enemy_cr)-creature_absolute_x(cr));
	return;
}

void ai_directives_aim_spin(blips_game *bgame,creature *cr)
{
	cr->aim_orientation+=.1;
	return;
}

void ai_directives_aim_lag_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	/* sine of difference in orientation and angle of vector to target
	 * determines whether to add or subtract to current orientation. */
	double target_angle,sine_of;

	target_angle=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
			   creature_absolute_x(enemy_cr)-creature_absolute_x(cr));

	sine_of=sin(cr->aim_orientation-target_angle);

	if(sine_of>0)
		cr->aim_orientation-=.04;
	else if(sine_of<0)
		cr->aim_orientation+=.04;
	return;
}
void ai_directives_aim_lead_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
/*UNFINISHED*/
	/* possible locations of fired projectile are a cone in hyperspace,
	 * and projected location of enemy is a hyperline.  Aim where they
	 * intersect. */
	return;
}

void ai_directives_aim_as_velocity(blips_game *bgame,creature *cr)
{
	cr->aim_orientation=cr->move_orientation;
	return;
}

void ai_directives_fire_spam(blips_game *bgame,creature *cr)
{
	cr->fire_cycle_state=(cr->fire_cycle_state+1)%(cr->type->fire_delay);
	return;
}

void ai_directives_fire_sight_first(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	/* if dot product of vector to target and orientation is near 1,
	 * fire */
	if(cos(cr->aim_orientation)*cos(creature_absolute_x(enemy_cr)-creature_absolute_x(cr))+
	   sin(cr->aim_orientation)*sin(creature_absolute_y(enemy_cr)-creature_absolute_y(cr))>.9)
		cr->fire_cycle_state=(cr->fire_cycle_state+1)%(cr->type->fire_delay);
	return;
}

void ai_directives_fire_spurt(blips_game *bgame,creature *cr,creature *enemy_cr)
{
/*UNFINISHED*/
	return;
}

