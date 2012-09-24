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

	/* Spawn any necessary projectiles */

		/* from non-player creatures */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->fire_cycle_state==0)
			blips_game_spawn_projectile_from_creature(bgame,bgame->creatures[i]);
		/* from player creatures */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->fire_cycle_state==0)
			blips_game_spawn_projectile_from_creature(bgame,bgame->players[i]);

	/* Move any projectiles that need moving */

	for(i=0;i<bgame->num_projectiles;i++)
		blips_game_move_projectile(bgame,bgame->projectiles[i]);

	/* Handle any projectile/creature, projectile/barrier collisions */
/* UNFINISHED -- write this code after AI, projectiles, etc. */

	/* projectile/boundary collisions */

	blips_game_remove_projectiles_outside_boundaries(bgame);

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


	bgame->world_tiles=(world_tile**)realloc(bgame->world_tiles,sizeof(world_tile*)*bgame->num_world_tiles+1);
	bgame->world_tiles[bgame->num_world_tiles]=world_tile_create(path);
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
				bgame->creatures[bgame->num_creatures]->team=1;

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
	projectile_type *pr_type;

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

	bgame->players=(creature**)malloc(sizeof(creature*)*bgame->campaign->num_players);
	bgame->player_types=(creature_type**)malloc(sizeof(creature_type*)*bgame->campaign->num_players);

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

void blips_game_apply_ai_type_to_creature(blips_game *bgame,ai_type *ai_type_ptr,creature *cr)
{
	double distance_squared,candidate_distance_squared;
	creature *enemy_cr;
	projectile *enemy_pr;
	double enemy_cr_abs_x,enemy_cr_abs_y;
	double enemy_pr_abs_x,enemy_pr_abs_y;
	double cr_abs_x,cr_abs_y;
	double candidate_x,candidate_y;
	int i;

/*UNFINISHED*/

	/* compute this creatures absolute position */

	cr_abs_x=creature_absolute_x(cr);
	cr_abs_y=creature_absolute_y(cr);

	/* get nearest enemy creature */

	distance_squared=10000000;

		/* search non-player creatures */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->team!=cr->team)
		{
			candidate_x=creature_absolute_x(bgame->creatures[i]);
			candidate_y=creature_absolute_y(bgame->creatures[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared)
			{
				enemy_cr=bgame->creatures[i];
				distance_squared=candidate_distance_squared;
			}
		}

		/* search player creatures */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->team!=cr->team)
		{
			candidate_x=creature_absolute_x(bgame->players[i]);
			candidate_y=creature_absolute_y(bgame->players[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared)
			{
				enemy_cr=bgame->players[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==10000000)
		enemy_cr=0;

	/* get nearest enemy projectile */

	distance_squared=10000000;

	for(i=0;i<bgame->num_projectiles;i++)
		if(bgame->projectiles[i]->team!=cr->team)
		{
			candidate_x=projectile_absolute_x(bgame->projectiles[i]);
			candidate_y=projectile_absolute_y(bgame->projectiles[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared)
			{
				enemy_pr=bgame->projectiles[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==10000000)
		enemy_pr=0;

	/* compute absolute positions of the nearest enemy cr
	 * and the nearest enemy pr */

	if(enemy_cr)
	{
		enemy_cr_abs_x=creature_absolute_x(enemy_cr);
		enemy_cr_abs_y=creature_absolute_y(enemy_cr);
	}

	if(enemy_pr)
	{
		enemy_pr_abs_x=projectile_absolute_x(enemy_pr);
		enemy_pr_abs_y=projectile_absolute_y(enemy_pr);
	}

	/* move goal affects creature's facing direction and speed */

	cr->current_move_speed=cr->type->move_speed;

	switch(ai_type_ptr->move_goal)
	{
		case AI_DODGE:
			if(enemy_pr)
			{
				/* if the cross product of the pr's vel and
				 * the vector from pr to cr has positive z,
				 * then we should ADD M_PI/2 from orientation */

				/* compute cross product's z */
				if(cos(enemy_pr->orientation)*(cr_abs_y-enemy_cr_abs_y)-
				   sin(enemy_pr->orientation)*(cr_abs_x-enemy_cr_abs_x)>0)
					cr->move_orientation=enemy_pr->orientation+M_PI/2.0;
				else
					cr->move_orientation=enemy_pr->orientation-M_PI/2.0;
			}
			else
				cr->current_move_speed=0;
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

	switch(ai_type_ptr->aim_goal)
	{
		case AI_FACE:
			if(enemy_cr)
				cr->aim_orientation=atan2(enemy_cr_abs_y-cr_abs_y,
							  enemy_cr_abs_x-cr_abs_x);
		break;
		case AI_SPIN:
			cr->aim_orientation+=.01;
		break;
		case AI_LAG_FACE:
		break;
	}

	/* fire goal affects creature's fire_cycle_state */

	switch(ai_type_ptr->fire_goal)
	{
		case AI_SPAM:
			cr->fire_cycle_state=(cr->fire_cycle_state+1)%(cr->type->fire_delay);
		break;
		case AI_SIGHT_FIRST:
		break;
		case AI_SPURT:
		break;
	}

	return;
}

void blips_game_move_creature(blips_game *bgame,creature *cr)
{
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
	}

	return;
}

void blips_game_move_projectile(blips_game *bgame,projectile *pr)
{
	double delta_x,delta_y;

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
	bgame->projectiles=(projectile**)realloc(bgame->projectiles,sizeof(projectile*)*(bgame->num_projectiles+1));
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
			projectile_destroy(bgame->projectiles[i]);
			bgame->projectiles[i]=bgame->projectiles[bgame->num_projectiles-1];
			bgame->projectiles=(projectile**)realloc(bgame->projectiles,sizeof(projectile*)*(bgame->num_projectiles-1));
			bgame->num_projectiles--;
			i--;
		}

	return;
}

int blips_game_creature_intersects_creatures(blips_game *bgame,creature *cr)
{
	int i;
	double distance_squared;

		/* non-player */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]!=cr)  /* don't check creature against itself */
		{
			distance_squared=pow(creature_absolute_x(bgame->creatures[i])-creature_absolute_x(cr),2)+
					 pow(creature_absolute_y(bgame->creatures[i])-creature_absolute_y(cr),2);
			if(distance_squared<BLIPS_TILE_SIZE*BLIPS_TILE_SIZE/4.0)
				return 1;
		}

		/* player */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]!=cr)  /* don't check creature against itself */
		{
			distance_squared=pow(creature_absolute_x(bgame->players[i])-creature_absolute_x(cr),2)+
					 pow(creature_absolute_y(bgame->players[i])-creature_absolute_y(cr),2);
			if(distance_squared<BLIPS_TILE_SIZE*BLIPS_TILE_SIZE/4.0)
				return 1;
		}

	return 0;
}

int blips_game_creature_intersects_boundaries(blips_game *bgame,creature *cr)
{
	if(cr->col<0 ||
	   cr->col>=BLIPS_TILE_COLS ||
	   cr->row<0 ||
	   cr->row>=BLIPS_TILE_ROWS)
		return 1;
	return 0;
}

int blips_game_creature_intersects_barriers(blips_game *bgame,creature *cr)
{
/*UNFINISHED*/
	return 0;
}

int blips_game_creature_intersects_breakables(blips_game *bgame,creature *cr)
{
/*UNFINISHED*/
	return 0;
}

