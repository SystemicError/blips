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

	/*** User-specified types ***/

	bgame->ai_types=0;
	bgame->num_ai_types=0;

	bgame->co_types=0;
	bgame->num_co_types=0;
	bgame->cr_types=0;
	bgame->num_cr_types=0;
	bgame->pr_types=0;
	bgame->num_pr_types=0;

	/*** Keyed types ***/

	bgame->br_types_map=string_map_create();
	bgame->co_type_key=0;
	bgame->cr_type_key=0;

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

	/*** User-specified types ***/

	for(i=0;i<bgame->num_ai_types;i++)
		ai_type_destroy(bgame->ai_types[i]);
	if(bgame->num_ai_types)
		free(bgame->ai_types);

	for(i=0;i<bgame->num_co_types;i++)
		collectible_type_destroy(bgame->co_types[i]);
	if(bgame->num_co_types)
		free(bgame->co_types);
	for(i=0;i<bgame->num_cr_types;i++)
		creature_type_destroy(bgame->cr_types[i]);
	if(bgame->num_cr_types)
		free(bgame->cr_types);
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

	for(i=0;i<bgame->num_co_types;i++)
		free(bgame->co_type_key[i]);
	if(bgame->num_co_types)
		free(bgame->co_type_key);
	for(i=0;i<bgame->num_cr_types;i++)
		free(bgame->cr_type_key[i]);
	if(bgame->num_cr_types)
		free(bgame->cr_type_key);

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
	return;
}

void blips_game_step(blips_game *bgame,blips_input_state *inputs)
{
	/*UNFINISHED*/
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

	string[2]=0;

	for(i=0;i<BLIPS_TILE_ROWS;i++)
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			/* Fetch a string from the world tile  */

			string[0]=bgame->active_world_tile->object_strings[i][j][0];
			string[1]=bgame->active_world_tile->object_strings[i][j][1];

			/* Compare the string to our object key
			 * and add an object of the type specified */

				/* search breakable types */
/* UNFINISHED */
				/* search collectible types */
/* UNFINISHED */
				/* search creature types */
/* UNFINISHED */
		}

	return;
}

void blips_game_load_object_types(blips_game *bgame)
{
	FILE *fp;
	int i,count;
	char *string;
	char buffer[BUFFER_SIZE];

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
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		fscanf(fp,"=%s\n",buffer);
		blips_game_add_collectible_type(bgame,buffer,string);
	}

	/*** Creature Types/Strings ***/
printf("Loading creature types . . .\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		string[0]=fgetc(fp);
		string[1]=fgetc(fp);
		fscanf(fp,"=%s\n",buffer);
		blips_game_add_creature_type(bgame,buffer,string);
	}

	fclose(fp);

	/*** AI Types ***/

	/* These have to be loaded from creature list, checked for duplicates. */

	/*UNFINISHED */

	/*** Projectile Types ***/

	/* These have to be loaded from creature list, checked for duplicates. */

	/*UNFINISHED*/

	return;
}

	/* load user_specified types */
void blips_game_add_collectible_type(blips_game *bgame,char *path,char *string)
{
char* ch;
	bgame->co_types=(collectible_type**)realloc(bgame->co_types,sizeof(collectible_type*)*(bgame->num_co_types+1));
	bgame->co_type_key=(char**)realloc(bgame->co_type_key,sizeof(char*)*(bgame->num_co_types+1));

	bgame->co_types[bgame->num_co_types]=collectible_type_create(path);
	bgame->co_type_key[bgame->num_co_types]=(char*)malloc(sizeof(char)*2);

	strcpy(bgame->co_type_key[bgame->num_co_types],string);

	bgame->num_co_types++;

	return;
}

void blips_game_add_creature_type(blips_game *bgame,char *path,char *string)
{
	bgame->cr_types=(creature_type**)realloc(bgame->cr_types,sizeof(creature_type*)*(bgame->num_cr_types+1));
	bgame->cr_type_key=(char**)realloc(bgame->cr_type_key,sizeof(char*)*(bgame->num_cr_types+1));

printf("Creating new creature_type.\n");
	bgame->cr_types[bgame->num_cr_types]=creature_type_create(path);
printf("created.\n");
	bgame->cr_type_key[bgame->num_cr_types]=(char*)malloc(sizeof(char)*2);

	strcpy(bgame->cr_type_key[bgame->num_cr_types],string);

	bgame->num_cr_types++;

	return;
}

