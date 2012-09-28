/* breakable_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"breakable_type.h"

breakable_type* breakable_type_create(char *path)
{
	breakable_type *br_type;
	FILE *fp;
	char buffer[BUFFER_SIZE];

	if(!(br_type=(breakable_type*)malloc(sizeof(breakable_type))))
	{
		fprintf(stderr,"Couldn't allocate breakable_type.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open breakable_type file:  %s.\n",path);
		exit(1);
	}
printf("Loading breakable_type from path %s.\n",path);

	/*** Toughness ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(br_type->toughness))!=1)
	{
		fprintf(stderr,"Couldn't parse toughness of breakable_type:  %s.\n",path);
		exit(1);
	}

	/*** Spawn Trigger ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	if(!strcmp(buffer,"enter"))
		br_type->trigger=SPAWN_ON_ENTRANCE;
	else if(!strcmp(buffer,"clear"))
		br_type->trigger=SPAWN_ON_CLEAR;
	else
	{
		fprintf(stderr,"Couldn't parse spawn trigger of breakable_type:  %s.\n",path);
		fprintf(stderr,"Read:  %s.\n",buffer);
		exit(1);
	}

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(br_type->respawn))!=1)
	{
		fprintf(stderr,"Couldn't parse respawn of breakable_type:  %s.\n",path);
		exit(1);
	}

	/*** Despawn delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(br_type->despawn_delay))!=1)
	{
		fprintf(stderr,"Couldn't parse despawn_delay of breakable_type:  %s.\n",path);
		exit(1);
	}

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;
	if(!(br_type->br_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate br_type->br_set_path.\n");
		exit(1);
	}
	strncpy(br_type->br_set_path,buffer,strlen(buffer)+1);
	br_type->br_set_path[strlen(buffer)]=0;

	/* We're done with the file, now. */
	fclose(fp);

	/*** Breakable Type (self) Path ***/
	if(!(br_type->br_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate br_type->br_type_path.\n");
		exit(1);
	}
	strncpy(br_type->br_type_path,path,strlen(path));
	br_type->br_type_path[strlen(path)]=0;

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

