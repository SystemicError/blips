/* collectible_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"collectible_type.h"

collectible_type* collectible_type_create(char *path)
{
	FILE *fp;
	collectible_type *co_type;
	char buffer[BUFFER_SIZE];

	co_type=(collectible_type*)malloc(sizeof(collectible_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open collectible_type file:  %s.\n",path);
		exit(1);
	}

	/*** Spawn Trigger ***/
printf("Collectible Spawn Trigger.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	if(strcmp(buffer,"enter"))
		co_type->trigger=SPAWN_ON_ENTRANCE;
	else if(strcmp(buffer,"clear"))
		co_type->trigger=SPAWN_ON_CLEAR;

	/*** Respawn ***/
printf("Collectible Respawn.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(co_type->respawn));

	/*** Media Set ***/
printf("Collectible Media Set.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	co_type->co_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
	strcpy(co_type->co_set_path,buffer);

	/*** Collectible Type (self) Path ***/
printf("Collectible Type self Path.\n");

	co_type->co_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1));
printf("Now copying path:  %s.\n",path);
	strcpy(co_type->co_type_path,path);
printf("Done copying.\n");

	fclose(fp);
printf("Returning from co_type_create().\n");
	return co_type;
}

void collectible_type_destroy(collectible_type *co_type)
{
	if(!co_type)
	{
		fprintf(stderr,"Attempt to free null collectible_type!\n");
		exit(1);
	}

printf("Freeing co media set path (currently reads:  %s).\n",co_type->co_set_path);
	free(co_type->co_set_path);
printf("Freeing co type path.\n");
	free(co_type->co_type_path);

printf("Freeing co_type.\n");
	free(co_type);

	return;
}

