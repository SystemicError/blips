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

world_tile* blips_game_active_world_tile(blips_game *bgame)
{
	return bgame->active_world_tile;
}
