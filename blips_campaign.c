/* blips_campaign.h */
/* Started 9/18/12 by Tevis Tsai */

#include"blips_campaign.h"

blips_campaign *blips_campaign_new(char *path)
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

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	bc->object_key_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(bc->object_key_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	bc->tile_image_key_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(bc->tile_image_key_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	bc->starting_world_tile_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(bc->starting_world_tile_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	bc->statics_list_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(bc->statics_list_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&(bc->num_players));

	bc->player_starting_rows=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_starting_cols=(int*)malloc(sizeof(int)*bc->num_players);
	bc->player_type_file_paths=(char**)malloc(sizeof(char*)*bc->num_players);

	for(i=0;i<bc->num_players;i++)
	{
		fscanf(fp,"%s\n",buffer);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_rows+i);
		fscanf(fp,"%s\n",buffer);  /* comment line */
		fscanf(fp,"%d\n",bc->player_starting_cols+i);

		fscanf(fp,"%s\n",buffer);  /* comment line */
		fscanf(fp,"%s\n",buffer);
		bc->player_type_file_paths[i]=(char*)malloc(sizeof(char)*strlen(buffer));
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

