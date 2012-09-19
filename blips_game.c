/* blips_game.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"blips_game.h"

blips_game* blips_game_create(void)
{
	blips_game *bgame;
	bgame=(blips_game*)malloc(sizeof(bgame));

	/*UNFINISHED*/

	return bgame;
}

void blips_game_destroy(blips_game *bgame)
{
	if(!bgame)
	{
		fprintf(stderr,"Attempt to free null blips_game!\n");
		exit(1);
	}

	/*UNFINISHED*/

	free(bgame);
	return;
}

/* externally called requests */

void blips_game_load_campaign(blips_game *bgame,char *path)
{
	bgame->campaign=blips_campaign_create(path);
	/*UNFINISHED? -- do we need to do more from here, or will constructor have handled things?*/
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

