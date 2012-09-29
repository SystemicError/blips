/* blips_campaign.h */
/* Started 9/18/12 by Tevis Tsai */

#include"blips_campaign.h"

blips_campaign *blips_campaign_create(char *path)
{
	FILE *fp;
	blips_campaign *bc;
	char buffer[BUFFER_SIZE];
	int i;

printf("Creating campaign . . .\n");
	if(!(bc=(blips_campaign*)malloc(sizeof(blips_campaign))))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign.\n");
		exit(1);
	}
printf("Opening file . . .\n");
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open blips_campaign file:  %s.\n",path);
		exit(1);
	}

	/*** Status Bar Image ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got status bar image path:  %s.\n",buffer);
	if(!(bc->status_bar_image_path=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->status_bar_image_path.\n");
		exit(1);
	}
	strncpy(bc->status_bar_image_path,buffer,BUFFER_SIZE);
	bc->status_bar_image_path[BUFFER_SIZE-1]=0;

	/*** Object Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got object key path:  %s.\n",buffer);
	if(!(bc->object_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->object_key_path.\n");
		exit(1);
	}
	strncpy(bc->object_key_path,buffer,BUFFER_SIZE);
	bc->object_key_path[BUFFER_SIZE-1]=0;

	/*** Tile Image Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got tile image key path:  %s.\n",buffer);
	if(!(bc->tile_image_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->tile_image_key_path.\n");
		exit(1);
	}
	strncpy(bc->tile_image_key_path,buffer,BUFFER_SIZE);
	bc->tile_image_key_path[BUFFER_SIZE-1]=0;

	/*** Starting World Tile ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got starting world tile path:  %s.\n",buffer);
	if(!(bc->starting_world_tile_path=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->starting_world_tile_path.\n");
		exit(1);
	}
	strncpy(bc->starting_world_tile_path,buffer,BUFFER_SIZE);
	bc->starting_world_tile_path[BUFFER_SIZE-1]=0;

	/*** Number of Players/Player Info ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(bc->num_players))!=1)
	{
		fprintf(stderr,"Error parsing number of players in campaign file:  %s.\n",path);
		exit(1);
	}
printf("Got number of players:  %d.\n",bc->num_players);

	if(!(bc->player_starting_rows=(int*)malloc(sizeof(int)*bc->num_players)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->player_starting_rows.\n");
		exit(1);
	}
	if(!(bc->player_starting_cols=(int*)malloc(sizeof(int)*bc->num_players)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->player_starting_cols.\n");
		exit(1);
	}
	if(!(bc->player_type_file_paths=(char**)malloc(sizeof(char*)*bc->num_players)))
	{
		fprintf(stderr,"Couldn't allocate blips_campaign->player_type_file_paths.\n");
		exit(1);
	}

	for(i=0;i<bc->num_players;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		if(fscanf(fp,"%d\n",bc->player_starting_rows+i)!=1)
		{
			fprintf(stderr,"Error parsing starting row of player %d in campaign file:  %s.\n",i,path);
			exit(1);
		}
printf("Got %d starting row:  %d.\n",i,bc->player_starting_rows[i]);
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		if(fscanf(fp,"%d\n",bc->player_starting_cols+i)!=1)
		{
			fprintf(stderr,"Error parsing starting col of player %d in campaign file:  %s.\n",i,path);
			exit(1);
		}
printf("Got %d starting col:  %d.\n",i,bc->player_starting_cols[i]);

		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got %d type file path:  %s.\n",i,buffer);
		if(!(bc->player_type_file_paths[i]=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
		{
			fprintf(stderr,"Couldn't allocate blips_campaign->player_type_file_paths[%d].\n",i);
			exit(1);
		}
		strncpy(bc->player_type_file_paths[i],buffer,BUFFER_SIZE);
		bc->player_type_file_paths[i][BUFFER_SIZE-1]=0;
	}

printf("Done loading campaign.\n");
	fclose(fp);
	return bc;
}

void blips_campaign_destroy(blips_campaign *bc)
{
	int i;
	if(!bc)
	{
		fprintf(stderr,"Attempt to free null blips_campaign!\n");
		exit(1);
	}
printf("Freeing object key path.\n");
	free(bc->object_key_path);
printf("Tile image key path.\n");
	free(bc->tile_image_key_path);
printf("Starting world tile path.\n");
	free(bc->starting_world_tile_path);
printf("Player type paths.\n");
	for(i=0;i<bc->num_players;i++)
		free(bc->player_type_file_paths[i]);
	if(bc->num_players)
	{
		free(bc->player_starting_rows);
		free(bc->player_starting_cols);
		free(bc->player_type_file_paths);
	}
	free(bc);
	return;
}

