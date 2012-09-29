/* blips_campaign.h */
/* Started 9/18/12 by Tevis Tsai */

#ifndef _BLIPS_CAMPAIGN_H
#define _BLIPS_CAMPAIGN_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _STRING_H
#include<string.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

typedef struct blips_campaign
{
	char *status_bar_image_path;
	char *object_key_path;
	char *tile_image_key_path;
	char *starting_world_tile_path;
	int num_players;
	int *player_starting_rows;
	int *player_starting_cols;
	char **player_type_file_paths;
} blips_campaign;

blips_campaign *blips_campaign_create(char *path);
void blips_campaign_destroy(blips_campaign *bc);

#endif

