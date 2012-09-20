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

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	if(strcmp(buffer,"enter"))
		co_type->trigger=SPAWN_ON_ENTRANCE;
	else if(strcmp(buffer,"clear"))
		co_type->trigger=SPAWN_ON_CLEAR;

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(co_type->respawn));

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	co_type->co_set_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(co_type->co_set_path,buffer);

	co_type->co_type_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(co_type->co_type_path,buffer);

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

