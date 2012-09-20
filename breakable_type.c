/* breakable_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"breakable_type.h"

breakable_type* breakable_type_create(char *path)
{
	breakable_type *br_type;
	FILE *fp;
	char buffer[BUFFER_SIZE];

	br_type=(breakable_type*)malloc(sizeof(breakable_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open breakable_type file:  %s.\n",path);
		exit(1);
	}

	/*** Toughness ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(br_type->toughness));

	/*** Spawn Trigger ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	if(strcmp(buffer,"enter"))
		br_type->trigger=SPAWN_ON_ENTRANCE;
	else if(strcmp(buffer,"clear"))
		br_type->trigger=SPAWN_ON_CLEAR;

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(br_type->respawn));

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	br_type->br_set_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(br_type->br_type_path,buffer);

	br_type->br_type_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(br_type->br_type_path,buffer);

	fclose(fp);
	return br_type;
}

void breakable_type_destroy(breakable_type *br_type)
{
	if(!br_type)
	{
		fprintf(stderr,"Attempt to free null breakable_type!\n");
		exit(1);
	}

	free(br_type->br_set_path);
	free(br_type->br_type_path);

	free(br_type);

	return;
}

