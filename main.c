/* main.c */
/* Started 9/14/12 by Tevis Tsai */

#include<stdio.h>

#include"blips_gui.h"
#include"blips_game.h"

int main(int argc,char *argv[])
{
	blips_gui *bgui;
	blips_game *bgame;

	printf("Creating blips game . . .\n");
	bgame=blips_game_create();

	printf("Loading campaign . . .\n");
	blips_game_load_campaign(bgame,"example_campaign/");

	printf("Creating GUI . . .\n");
	bgui=blips_gui_create(bgame);

	printf("Starting GUI main loop . . .\n");
	blips_gui_main_loop(bgui);

	printf("Destroying GUI . . .\n");
	blips_gui_destroy(bgui);

	printf("Destroying game . . .\n");
	blips_game_destroy(bgame);

	return 0;
}

