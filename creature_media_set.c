/* creature_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"creature_media_set.h"

creature_media_set* creature_media_set_create(char *path)
{
	creature_media_set *cr_set;
	char buffer[BUFFER_SIZE];
	FILE *fp;
	int i,count;

	if(!(cr_set=(creature_media_set*)malloc(sizeof(creature_media_set))))
	{
		fprintf(stderr,"Couldn't allocate creature_media_set.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open creature_media_set:  %s.\n",path);
		exit(1);
	}

	cr_set->stand_animation=sprite_animation_create();
	cr_set->walk_animation=sprite_animation_create();
	cr_set->walk_and_fire_animation=sprite_animation_create();
	cr_set->fire_animation=sprite_animation_create();
	cr_set->stun_animation=sprite_animation_create();

	/*** STAND animations ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of STAND animations in creature_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(cr_set->stand_animation,buffer);
	}

	/*** WALK animations ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of WALK animations in creature_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(cr_set->walk_animation,buffer);
	}

	/*** WALK_AND_FIRE animations ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of WALK_AND_FIRE animations in creature_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(cr_set->walk_and_fire_animation,buffer);
	}

	/*** FIRE animations ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of FIRE animations in creature_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(cr_set->fire_animation,buffer);
	}

	/*** STUN animations ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of STUN animations in creature_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(cr_set->stun_animation,buffer);
	}

	/*** Sounds ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->stand_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->stand_sound=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->walk_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->walk_sound=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->walk_and_fire_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->walk_and_fire_sound=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->fire_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		cr_set->fire_sound=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(cr_set->stun_sound=Mix_LoadWAV(buffer)))
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

