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
	bc=(blips_campaign*)malloc(sizeof(blips_campaign));
printf("Opening file . . .\n");
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open blips_campaign file:  %s.\n",path);
		exit(1);
	}

	/*** Object Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got object key path:  %s.\n",buffer);
	bc->object_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->object_key_path,buffer);

	/*** Tile Image Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got tile image key path:  %s.\n",buffer);
	bc->tile_image_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->tile_image_key_path,buffer);

	/*** Starting World Tile ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got starting world tile path:  %s.\n",buffer);
	bc->starting_world_tile_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->starting_world_tile_path,buffer);

	/*** Statics List ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got statics list path:  %s.\n",buffer);
	bc->statics_list_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->statics_list_path,buffer);

	/*** Number of Players/Player Info ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(bc->num_players));
printf("Got number of players:  %d.\n",bc->num_players);

	bc->player_starting_rows=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_starting_cols=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_type_file_paths=(char**)malloc(sizeof(char*)*bc->num_players);

	for(i=0;i<bc->num_players;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_rows+i);
printf("Got %d starting row:  %d.\n",i,bc->player_starting_rows[i]);
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_cols+i);
printf("Got %d starting col:  %d.\n",i,bc->player_starting_cols[i]);

		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Got %d type file path:  %s.\n",i,buffer);
		bc->player_type_file_paths[i]=(char*)malloc(sizeof(char)*BUFFER_SIZE);
		strcpy(bc->player_type_file_paths[i],buffer);
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
printf("Statics list path.\n");
	free(bc->statics_list_path);
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

