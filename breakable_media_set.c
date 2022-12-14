/* breakable_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"breakable_media_set.h"

breakable_media_set* breakable_media_set_create(char *path)
{
	breakable_media_set *br_set;
	char buffer[BUFFER_SIZE];
	FILE *fp;
	int i,count;

	if(!(br_set=(breakable_media_set*)malloc(sizeof(breakable_media_set))))
	{
		fprintf(stderr,"Couldn't allocate breakable_media_set.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open breakable_media_set:  %s.\n",path);
		exit(1);
	}

	br_set->stand_animation=sprite_animation_create();
	br_set->break_animation=sprite_animation_create();

	/*** STAND animation ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of STAND animation frames in breakable_media_set of path:  %s.\n",path);
		exit(1);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(br_set->stand_animation,buffer);
	}

	/*** BREAK animation ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of BREAK animation frames in breakable_media_set of path:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(br_set->break_animation,buffer);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(br_set->break_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		br_set->break_sound=0;

	fclose(fp);
	return br_set;
}

void breakable_media_set_destroy(breakable_media_set *br_set)
{
	if(!br_set)
	{
		fprintf(stderr,"Attempt to free null breakable_media_set!\n");
		exit(1);
	}
	sprite_animation_destroy(br_set->stand_animation);
	sprite_animation_destroy(br_set->break_animation);
	if(br_set->break_sound)
		Mix_FreeChunk(br_set->break_sound);
	free(br_set);
	return;
}

