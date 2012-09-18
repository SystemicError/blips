/* creature_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"creature_media_set.h"

creature_media_set* creature_media_set_new(char *path)
{
	creature_media_set *cr_set;
	char buffer[1024];
	FILE *fp;
	int i,count;

	cr_set=(creature_media_set*)malloc(sizeof(creature_media_set));
	if(!(file=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open creature_media_set:  %s.\n",path);
		exit(1);
	}

	cr_set->stand_animation=sprite_animation_new();
	cr_set->walk_animation=sprite_animation_new();
	cr_set->walk_and_fire_animation=sprite_animation_new();
	cr_set->fire_animation=sprite_animation_new();
	cr_set->stun_animation=sprite_animation_new();

	/*** Animations ***/

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(cr_set->stand_animation,buffer);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(cr_set->walk_animation,buffer);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(cr_set->walk_and_fire_animation,buffer);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(cr_set->fire_animation,buffer);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&count);
	fscanf(fp,"%s\n",buffer);  /* comment line */
	for(i=0;i<count;i++)
	{
		fscanf(fp,"%s\n",buffer);
		sprite_animation_add_frame(cr_set->stun_animation,buffer);
	}

	/*** Sounds ***/

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->stand_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->stand_sound=0;

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->walk_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->walk_sound=0;

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->walk_and_fire_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->walk_and_fire_sound=0;

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->fire_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->fire_sound=0;

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->stun_sound=Mix_LoadWav(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->stun_sound=0;

	fclose(fp);
	return cr_set;
}

void creature_media_set_destroy(creature_media_set *cr_set)
{
	if(!cr_set)
	{
		fprintf(stderr,"Attempt to free null creature_media_set!\n");
		exit(1);
	}
	sprite_animation_destroy(cr_set->stand_animation);
	sprite_animation_destroy(cr_set->walk_animation);
	sprite_animation_destroy(cr_set->walk_and_fire_animation);
	sprite_animation_destroy(cr_set->fire_animation);
	sprite_animation_destroy(cr_set->stun_animation);
	if(cr_set->stand_sound)
		Mix_FreeChunk(cr_set->stand_sound);
	if(cr_set->walk_sound)
		Mix_FreeChunk(cr_set->walk_sound);
	if(cr_set->walk_and_fire_sound)
		Mix_FreeChunk(cr_set->walk_and_fire_sound);
	if(cr_set->fire_sound)
		Mix_FreeChunk(cr_set->fire_sound);
	if(cr_set->stun_sound)
		Mix_FreeChunk(cr_set->stun_sound);
	free(cr_set);
	return;
}

