/* creature_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"creature_type.h"

creature_type* creature_type_create(char *path)
{
	FILE *fp;
	creature_type *cr_type;
	char buffer[BUFFER_SIZE];

	if(!(cr_type=(creature_type*)malloc(sizeof(creature_type))))
	{
		fprintf(stderr,"Couldn't allocate creature_type.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open creature_type file:  %s.\n",path);
		exit(1);
	}
printf("Loading creature type of path %s.\n",path);

	/*** Max Health ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->max_health))!=1)
	{
		fprintf(stderr,"Couldn't parse max_health of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** Projectile Type ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(cr_type->pr_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate creature_type->pr_type_path.\n",path);
		exit(1);
	}
	strncpy(cr_type->pr_type_path,buffer,strlen(buffer));
	cr_type->pr_type_path[strlen(buffer)]=0;

	/*** Move Speed ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->move_speed))!=1)
	{
		fprintf(stderr,"Couldn't parse move_speed of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** Fire Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->fire_delay))!=1)
	{
		fprintf(stderr,"Couldn't parse fire_delay of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** Stun Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->stun_delay))!=1)
	{
		fprintf(stderr,"Couldn't parse stun_delay of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** AI Type ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(cr_type->ai_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate creature_type->ai_type_path.\n");
		exit(1);
	}
	strncpy(cr_type->ai_type_path,buffer,strlen(buffer));
	cr_type->ai_type_path[strlen(buffer)]=0;
printf("Got AI type path:  %s.\n",cr_type->ai_type_path);

	/*** Spawn Trigger ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!strcmp(buffer,"enter"))
		cr_type->trigger=SPAWN_ON_ENTRANCE;
	else if(!strcmp(buffer,"clear"))
		cr_type->trigger=SPAWN_ON_CLEAR;
	else
	{
		fprintf(stderr,"Couldn't parse spawn_trigger of creature_type:  %s.\n",path);
		fprintf(stderr,"Read:  %s.\n",buffer);
		exit(1);
	}

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->respawn))!=1)
	{
		fprintf(stderr,"Couldn't parse respawn of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** Despawn Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(cr_type->despawn_delay))!=1)
	{
		fprintf(stderr,"Couldn't parse despawn_delay of creature_type:  %s.\n",path);
		exit(1);
	}

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(cr_type->cr_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate creature_type->cr_set_path.\n");
		exit(1);
	}
	strncpy(cr_type->cr_set_path,buffer,strlen(buffer));
	cr_type->cr_set_path[strlen(buffer)]=0;
printf("Got media set path:  %s.\n",cr_type->cr_set_path);

	if(!(cr_type->cr_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate creature_type->cr_type_path.\n");
		exit(1);
	}
	strncpy(cr_type->cr_type_path,path,strlen(path));
	cr_type->cr_type_path[strlen(path)]=0;

	fclose(fp);
	return cr_type;
}

void creature_type_destroy(creature_type *cr_type)
{
	if(!cr_type)
	{
		fprintf(stderr,"Attempt to free null creature_type!\n");
		exit(1);
	}

	free(cr_type->pr_type_path);
	free(cr_type->ai_type_path);
	free(cr_type->cr_set_path);
	free(cr_type->cr_type_path);

	free(cr_type);

	return;
}

