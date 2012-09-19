/* blips_campaign.h */
/* Started 9/18/12 by Tevis Tsai */

#include"blips_campaign.h"

blips_campaign *blips_campaign_create(char *path)
{
	FILE *fp;
	blips_campaign *bc;
	char buffer[1024];
	char full_path[1024];
	int i;

	strcpy(full_path,path);
	strcat(full_path,"/campaign");

	bc=(blips_campaign*)malloc(sizeof(blips_campaign));
	if(!(fp=fopen(full_path,"r")))
	{
		fprintf(stderr,"Couldn't open blips_campaign file:  %s.\n",full_path);
		exit(1);
	}

	bc->path_prefix=(char*)malloc(sizeof(char)*strlen(path));
	strcpy(bc->path_prefix,path);

	/*** Object Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
printf("Got object key path:  %s.\n",buffer);
	bc->object_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->object_key_path,buffer);

	/*** Tile Image Key ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
printf("Got tile image key path:  %s.\n",buffer);
	bc->tile_image_key_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->tile_image_key_path,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	bc->starting_world_tile_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->starting_world_tile_path,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	bc->statics_list_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(bc->statics_list_path,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(bc->num_players));

	bc->player_starting_rows=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_starting_cols=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_type_file_paths=(char**)malloc(sizeof(char*)*bc->num_players);

	for(i=0;i<bc->num_players;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_rows+i);
		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_cols+i);

		fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
		fgets(buffer,BUFFER_SIZE,fp);
		bc->player_type_file_paths[i]=(char*)malloc(sizeof(char)*BUFFER_SIZE);
		strcpy(bc->player_type_file_paths[i],buffer);
	}

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
	free(bc->object_key_path);
	free(bc->tile_image_key_path);
	free(bc->starting_world_tile_path);
	free(bc->statics_list_path);
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

