/* creature_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"creature_type.h"

creature_type* creature_type_create(char *path)
{
	FILE *fp;
	creature_type *cr_type;
	char buffer[BUFFER_SIZE];

	cr_type=(creature_type*)malloc(sizeof(creature_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open creature_type file:  %s.\n",path);
		exit(1);
	}

	/*** Max Health ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->max_health));

	/*** Projectile Type ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	cr_type->pr_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
	strcpy(cr_type->pr_type_path,buffer);

	/*** Move Speed ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->move_speed));

	/*** Fire Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->fire_delay));

	/*** AI Type ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	cr_type->ai_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
	strcpy(cr_type->ai_type_path,buffer);

	/*** Spawn Trigger ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	if(strcmp(buffer,"enter"))
		cr_type->trigger=SPAWN_ON_ENTRANCE;
	else if(strcmp(buffer,"clear"))
		cr_type->trigger=SPAWN_ON_CLEAR;

	/*** Respawn ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->respawn));

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	cr_type->cr_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
	strcpy(cr_type->cr_set_path,buffer);

	cr_type->cr_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1));
	strcpy(cr_type->cr_type_path,path);

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

