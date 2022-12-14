/* collectible_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"collectible_media_set.h"

collectible_media_set* collectible_media_set_create(char *path)
{
	collectible_media_set *co_set;
	char buffer[BUFFER_SIZE];
	FILE *fp;
	int i,count;

	if(!(co_set=(collectible_media_set*)malloc(sizeof(collectible_media_set))))
	{
		fprintf(stderr,"Couldn't allocate collectible_media_set.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open collectible_media_set:  %s.\n",path);
		exit(1);
	}

	co_set->stand_animation=sprite_animation_create();

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of STAND animation frames in collectible_media_set:  %s.\n",path);
		exit(1);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(co_set->stand_animation,buffer);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(co_set->pickup_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		co_set->pickup_sound=0;

	fclose(fp);
	return co_set;
}

void collectible_media_set_destroy(collectible_media_set *co_set)
{
	if(!co_set)
	{
		fprintf(stderr,"Attempt to free null collectible_media_set!\n");
		exit(1);
	}
	sprite_animation_destroy(co_set->stand_animation);
	if(co_set->pickup_sound)
		Mix_FreeChunk(co_set->pickup_sound);
	free(co_set);
	return;
}

