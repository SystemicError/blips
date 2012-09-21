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
printf("Max Health.\n");
char *chtest2;
chtest2=(char*)malloc(sizeof(char));
free(chtest2);
printf("pass test here.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->max_health));

	/*** Projectile Type ***/
printf("Projectile.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
printf("Projectile comment:  %s.\n",buffer);
	fgets(buffer,BUFFER_SIZE,fp);
printf("Projectile read:  %s.\n",buffer);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("Projectile read (post):  %s.\n",buffer);
	cr_type->pr_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
printf("Copying buffer to pr_type_path (size %d).\n",strlen(buffer));
printf("%s.\n",buffer);
strcpy(cr_type->pr_type_path,"HEY!");
	strcpy(cr_type->pr_type_path,buffer);
printf("The copied pr_type_path:  %s.\n",cr_type->pr_type_path);

	/*** Move Speed ***/
printf("Move speed.\n");
char *chtest;
chtest=(char*)malloc(sizeof(char));
free(chtest);
printf("pass test here.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->move_speed));

	/*** Fire Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(cr_type->fire_delay));

	/*** AI Type ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
printf("AD path %s buffer %s len %d\n",path,buffer,strlen(buffer));
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("ADE\n");
	cr_type->ai_type_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
printf("AE\n");
	strcpy(cr_type->ai_type_path,buffer);

	/*** Spawn Trigger ***/
printf("BA\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	if(strcmp(buffer,"enter"))
		cr_type->trigger=SPAWN_ON_ENTRANCE;
	else if(strcmp(buffer,"clear"))
		cr_type->trigger=SPAWN_ON_CLEAR;

	/*** Respawn ***/
printf("Respawn.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
printf("Respawn comment:  %s.\n",buffer);
	fscanf(fp,"%d\n",&(cr_type->respawn));

	/*** Media Set ***/
printf("Media set.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
printf("Media set comment:  %s.\n",buffer);
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
printf("media path:  %s.\n",buffer);
	cr_type->cr_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1));
	strcpy(cr_type->cr_set_path,buffer);

printf("Cr type path.\n");
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

