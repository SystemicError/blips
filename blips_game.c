/* blips_game.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"blips_game.h"

blips_game* blips_game_create(void)
{
	blips_game *bgame;
	bgame=(blips_game*)malloc(sizeof(blips_game));

	/*** World tiles ***/

	bgame->world_tiles=0;
	bgame->num_world_tiles=0;

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

	bgame->pr_types=0;
	bgame->num_pr_types=0;

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

	for(i=0;i<bgame->num_world_tiles;i++)
		world_tile_destroy(bgame->world_tiles[i]);
	if(bgame->num_world_tiles)
		free(bgame->world_tiles);

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

	for(i=0;i<bgame->num_pr_types;i++)
		projectile_type_destroy(bgame->pr_types[i]);
	if(bgame->num_pr_types)
		free(bgame->pr_types);

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
	blips_game_load_object_types(bgame);

	blips_game_load_players(bgame);

	/* for first tile spawn */
	blips_game_spawn(bgame,SPAWN_ON_ENTRANCE);
	return;
}

void blips_game_step(blips_game *bgame,blips_input_state **inputs)
{
	int i;
	char *str;
	ai_type *ai_type_ptr;
/*UNFINISHED*/
	/* Compute AI's commands for each creature */

	for(i=0;i<bgame->num_creatures;i++)
	{
		str=bgame->creatures[i]->type->ai_type_path;
		string_map_string_to_pointer(bgame->ai_types_map,str,(void**)&ai_type_ptr);
		blips_game_apply_ai_type_to_creature(bgame,ai_type_ptr,bgame->creatures[i]);
	}

	/* Use inputs as commands for player creature */

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

	/* Move any creatures that need moving */

		/* non-player */
	for(i=0;i<bgame->num_creatures;i++)
		blips_game_move_creature(bgame,bgame->creatures[i]);

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
		blips_game_move_creature(bgame,bgame->players[i]);

	/* Handle any creature/barrier, creature/breakable, creature/creature collisions */

	/* Spawn any necessary projectiles */

	/* Move any projectiles that need moving */

	/* Handle any projectile/creature, projectile/barrier collisions */

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

printf("Loading world tiles . . .\n");
	if(bgame->num_world_tiles)
	{
		fprintf(stderr,"Attempt to load world tile cache into already loaded game!\n");
		exit(1);
	}

printf("Adding first world tile . . .\n");
	blips_game_add_world_tile(bgame,bgame->campaign->starting_world_tile_path);
printf("First world tile added.\n");
	bgame->active_world_tile=bgame->world_tiles[0];

	/* Terribly inefficient, I know.  We only do this once.  */
	do
	{
printf("Entering loop.\n");
		updated=0;
		for(i=0;i<bgame->num_world_tiles;i++)
		{
			if((strcmp("none",bgame->world_tiles[i]->north_tile) && blips_game_add_world_tile(bgame,bgame->world_tiles[i]->north_tile)) ||
			   (strcmp("none",bgame->world_tiles[i]->east_tile) && blips_game_add_world_tile(bgame,bgame->world_tiles[i]->east_tile)) ||
			   (strcmp("none",bgame->world_tiles[i]->south_tile) && blips_game_add_world_tile(bgame,bgame->world_tiles[i]->south_tile)) ||
			   (strcmp("none",bgame->world_tiles[i]->west_tile) && blips_game_add_world_tile(bgame,bgame->world_tiles[i]->west_tile)))
			updated=1;
		}
	}while(updated);
printf("Left loop.\n");

	return;
}

int blips_game_add_world_tile(blips_game *bgame,char *path)
{
	/*** Adds a single world tile to the world_tiles array.  Returns
	 *** non-zero if added, zero if a rejected duplicate.  */

	int i;

printf("Got request to add world tile of path:  %s.\n",path);

	/* scan for duplication */
	for(i=0;i<bgame->num_world_tiles;i++)
		if(!strcmp(bgame->world_tiles[i]->path,path))
			return 0;

printf("Passed duplicate search, is unique against extant %d tiles.\n",bgame->num_world_tiles);

	bgame->world_tiles=(world_tile**)realloc(bgame->world_tiles,sizeof(world_tile*)*bgame->num_world_tiles+1);
printf("Allocated ptr.  Creating.\n");
	bgame->world_tiles[bgame->num_world_tiles]=world_tile_create(path);
printf("Incrementing count.\n");
	bgame->num_world_tiles++;
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
	for(i=0;i<bgame->num_collectibles;i++)
		collectible_destroy(bgame->collectibles[i]);
	if(bgame->num_collectibles)
		free(bgame->collectibles);
	for(i=0;i<bgame->num_creatures;i++)
		creature_destroy(bgame->creatures[i]);
/* UNFINISHED -- player creature should be kept! */
	if(bgame->num_creatures)
		free(bgame->creatures);

	for(i=0;i<bgame->num_projectiles;i++)
		projectile_destroy(bgame->projectiles[i]);
	if(bgame->num_projectiles)
		free(bgame->projectiles);

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
				bgame->breakables=(breakable**)realloc(bgame->breakables,sizeof(breakable*)*(bgame->num_breakables+1));
				bgame->breakables[bgame->num_breakables]=breakable_create(br_type);

				bgame->breakables[bgame->num_breakables]->row=i;
				bgame->breakables[bgame->num_breakables]->col=j;

				bgame->num_breakables++;
			}

				/* search collectible types */
			string_map_string_to_pointer(bgame->co_types_map,string,(void**)&co_type);

			if(co_type && co_type->trigger==trigger)
			{
				bgame->collectibles=(collectible**)realloc(bgame->collectibles,sizeof(collectible*)*(bgame->num_collectibles+1));
				bgame->collectibles[bgame->num_collectibles]=collectible_create(co_type);

				bgame->collectibles[bgame->num_collectibles]->row=i;
				bgame->collectibles[bgame->num_collectibles]->col=j;

				bgame->num_collectibles++;
			}

				/* search creature types */
			string_map_string_to_pointer(bgame->cr_types_map,string,(void**)&cr_type);

			if(cr_type && cr_type->trigger==trigger)
			{
				bgame->creatures=(creature**)realloc(bgame->creatures,sizeof(creature*)*(bgame->num_creatures+1));
				bgame->creatures[bgame->num_creatures]=creature_create(cr_type);

				bgame->creatures[bgame->num_creatures]->row=i;
				bgame->creatures[bgame->num_creatures]->col=j;

				bgame->num_creatures++;
			}
		}

	return;
}

void blips_game_load_object_types(blips_game *bgame)
{
	FILE *fp;
	int i,count;
	char *string;
	char buffer[BUFFER_SIZE];
	ai_type *ai_type_ptr;

	if(!(fp=fopen(bgame->campaign->object_key_path,"r")))
	{
		fprintf(stderr,"Couldn't open object_key path:  %s\n.",bgame->campaign->object_key_path);
		exit(1);
	}


	/*** Breakable Types/Strings ***/
printf("Loading breakable types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		string=(char*)malloc(sizeof(char)*3);
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fscanf(fp,"=%s\n",buffer);
		string_map_add(bgame->br_types_map,
				string,
				(void*)breakable_type_create(buffer));
	}

	/*** Collectible Types/Strings ***/
printf("Loading collectible types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		string=(char*)malloc(sizeof(char)*3);
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fscanf(fp,"=%s\n",buffer);
		string_map_add(bgame->co_types_map,
				string,
				(void*)collectible_type_create(buffer));
	}

	/*** Creature Types/Strings ***/
printf("Loading creature types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		string=(char*)malloc(sizeof(char)*3);
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		string[2]=0;
		fscanf(fp,"=%s\n",buffer);
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

	/* These have to be loaded from creature list, checked for duplicates. */

	/*UNFINISHED*/
printf("Loading of projectile types not implemented!\n");

	return;
}

void blips_game_load_players(blips_game *bgame)
{
	int i;

	bgame->players=(creature**)malloc(sizeof(creature*)*bgame->campaign->num_players);
	bgame->player_types=(creature_type**)malloc(sizeof(creature_type*)*bgame->campaign->num_players);

	for(i=0;i<bgame->campaign->num_players;i++)
	{
		bgame->player_types[i]=creature_type_create(bgame->campaign->player_type_file_paths[i]);
		bgame->players[i]=creature_create(bgame->player_types[i]);

		bgame->players[i]->row=bgame->campaign->player_starting_rows[i];
		bgame->players[i]->col=bgame->campaign->player_starting_cols[i];
	}

	return;
}

void blips_game_apply_ai_type_to_creature(blips_game *bgame,ai_type *ai_type_ptr,creature *cr)
{
/*UNFINISHED*/
	/* move goal affects creature's facing direction and speed */

cr->current_move_speed=1;
cr->move_orientation=.5*M_PI;
	switch(ai_type_ptr->move_goal)
	{
		case AI_DODGE:
		break;
		case AI_FLEE:
		break;
		case AI_CLOSE:
		break;
		case AI_WANDER:
		break;
		case AI_PATROL_NS:
		break;
		case AI_PATROL_EW:
		break;
	}

	/* aim goal affects creature's aim */

	/* fire goal affects creature's fire_cycle_state */

	return;
}

blips_game_move_creature(blips_game *bgame,creature *cr)
{
/* UNFINISHED -- needs to take into account collisions */
	int delta_x,delta_y;

	delta_x=(int)(cr->current_move_speed*cos(cr->move_orientation));
	delta_y=(int)(cr->current_move_speed*sin(cr->move_orientation));

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

	return;
}

