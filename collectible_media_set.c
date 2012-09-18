/* collectible_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"collectible_media_set.h"

collectible_media_set* collectible_media_set_new(char *path)
{
	collectible_media_set *co_set;
	char buffer[1024];
	FILE *fp;
	int i,count;

	co_set=(collectible_media_set*)malloc(sizeof(collectible_media_set));
	if(!(file=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open collectible_media_set:  %s.\n",path);
		exit(1);
	}

	co_set->stand_animation=sprite_animation_new();

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(co_set->stand_animation,buffer);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(co_set->pickup_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		co_set->pickup_sound=0;

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
