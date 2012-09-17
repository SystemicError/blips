/* main.c */
/* Started 9/14/12 by Tevis Tsai */

#include<stdio.h>

#include"blips_gui.h"
#include"blips_campaign.h"

int main(int argc,char *argv[])
{
	blips_gui *bgui;
	blips_game *bgame;

	bgame=blips_game_create();

	blips_game_load_campaign(bgame,"example_campaign/");

	bgui=blips_gui_create(bgame);

	blips_gui_main_loop(bgui);

	blips_gui_destroy(bgui);
	blips_game_destroy(bgame);

	return 0;
}

