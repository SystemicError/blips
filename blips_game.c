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
//	bgame->breakables=0;
	bgame->num_breakables=0;
//	bgame->collectibles=0;
	bgame->num_collectibles=0;
//	bgame->creatures=0;
	bgame->num_creatures=0;
//	bgame->projectiles=0;
	bgame->num_projectiles=0;

	/*** User-specified types ***/

	bgame->ai_types=0;
	bgame->num_ai_types=0;
	bgame->br_types=0;
	bgame->num_br_types=0;
	bgame->co_types=0;
	bgame->num_co_types=0;
	bgame->cr_types=0;
	bgame->num_cr_types=0;
	bgame->pr_types=0;
	bgame->num_pr_types=0;

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

	for(i=0;i<bgame->num_world_tiles;i++)
		world_tile_destroy(bgame->world_tiles[i]);
	if(bgame->num_world_tiles)
		free(bgame->world_tiles);

	/*** Instances of user-specified types ***/

/*	for(i=0;i<bgame->num_ais;i++)
		ai_destroy(bgame->ais[i]);
	if(bgame->num_ais)
		free(bgame->ais);
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

	/*** User-specified types ***/

	for(i=0;i<bgame->num_ai_types;i++)
		ai_type_destroy(bgame->ai_types[i]);
	if(bgame->num_ais)
		free(bgame->ai_types);
	for(i=0;i<bgame->num_br_types;i++)
		breakable_type_destroy(bgame->br_types[i]);
	if(bgame->num_br_types)
		free(bgame->br_types);
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

	blips_game_add_world_tile(bgame,bgame->campaign->starting_world_tile_path);

	/* Terribly inefficient, I know.  We only do this once.  */
	do
	{
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

