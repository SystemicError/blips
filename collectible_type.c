/* collectible_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"collectible_type.h"

collectible_type* collectible_type_create(char *path)
{
	FILE *fp;
	collectible_type *co_type;
	char buffer[BUFFER_SIZE];

	if(!(co_type=(collectible_type*)malloc(sizeof(collectible_type))))
	{
		fprintf(stderr,"Couldn't allocate collectible_type.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open collectible_type file:  %s.\n",path);
		exit(1);
	}
printf("Loading collectible_type of path:  %s.\n",path);

	/*** Spawn Trigger ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!strcmp(buffer,"enter"))
		co_type->trigger=SPAWN_ON_ENTRANCE;
	else if(!strcmp(buffer,"clear"))
		co_type->trigger=SPAWN_ON_CLEAR;
	else
	{
		fprintf(stderr,"Couldn't parse spawn trigger of collectible_type:  %s.\n",path);
		fprintf(stderr,"Read:  %s.\n",buffer);
		exit(1);
	}

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(co_type->respawn))!=1)
	{
		fprintf(stderr,"Couldn't parse respawn of collectible_type:  %s.\n",path);
		exit(1);
	}

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	if(!(co_type->co_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate collectible_type->co_set_path.\n");
		exit(1);
	}
	strncpy(co_type->co_set_path,buffer,strlen(buffer));
	co_type->co_set_path[strlen(buffer)]=0;

	/*** Collectible Type (self) Path ***/

	if(!(co_type->co_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate collectible_type->co_type_path.\n");
		exit(1);
	}
	strncpy(co_type->co_type_path,path,strlen(path));
	co_type->co_type_path[strlen(path)]=0;

	fclose(fp);
	return co_type;
}

void collectible_type_destroy(collectible_type *co_type)
{
	if(!co_type)
	{
		fprintf(stderr,"Attempt to free null collectible_type!\n");
		exit(1);
	}

	free(co_type->co_set_path);
	free(co_type->co_type_path);

	free(co_type);

	return;
}

