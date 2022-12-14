/* projectile_media_set.c */
/* Started 9/18/12 by Tevis Tsai. */

#include"projectile_media_set.h"

projectile_media_set* projectile_media_set_create(char *path)
{
	projectile_media_set *pr_set;
	char buffer[BUFFER_SIZE];
	FILE *fp;
	int i,count;

	if(!(pr_set=(projectile_media_set*)malloc(sizeof(projectile_media_set))))
	{
		fprintf(stderr,"Couldn't allocate projectile_media_set.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open projectile_media_set:  %s.\n",path);
		exit(1);
	}

	pr_set->fly_animation=sprite_animation_create();
	pr_set->impact_animation=sprite_animation_create();

	/*** FLY Animation ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of FLY animation frames in projectile_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(pr_set->fly_animation,buffer);
	}

	/*** IMPACT Animation ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&count)!=1)
	{
		fprintf(stderr,"Couldn't parse number of FLY animation frames in projectile_media_set:  %s.\n",path);
		exit(1);
	}
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<count;i++)
	{
		fgets(buffer,BUFFER_SIZE,fp);
		buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
		sprite_animation_add_frame(pr_set->impact_animation,buffer);
	}

	/*** Sounds ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(strcmp(buffer,"none"))
	{
		if(!(pr_set->impact_sound=Mix_LoadWAV(buffer)))
		{
			fprintf(stderr,"Error loading sample:  %s.\n",buffer);
			exit(1);
		}
	}
	else
		pr_set->impact_sound=0;

	fclose(fp);
	return pr_set;
}

void projectile_media_set_destroy(projectile_media_set *pr_set)
{
	if(!pr_set)
	{
		fprintf(stderr,"Attempt to free null projectile_media_set!\n");
		exit(1);
	}
	sprite_animation_destroy(pr_set->fly_animation);
	sprite_animation_destroy(pr_set->impact_animation);
	if(pr_set->impact_sound)
		Mix_FreeChunk(pr_set->impact_sound);
	free(pr_set);
	return;
}

