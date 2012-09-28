/* projectile_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"projectile_type.h"

projectile_type* projectile_type_create(char *path)
{
	FILE *fp;
	projectile_type *pr_type;
	char buffer[BUFFER_SIZE];

	if(!(pr_type=(projectile_type*)malloc(sizeof(projectile_type))))
	{
		fprintf(stderr,"Couldn't allocate projectile_type.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open projectile_type file:  %s.\n",path);
		exit(1);
	}

	/*** Damage ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(pr_type->damage))!=1)
	{
		fprintf(stderr,"Couldn't parse damage of projectile_type:  %s.\n");
		exit(1);
	}

	/*** Move Speed ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(pr_type->move_speed))!=1)
	{
		fprintf(stderr,"Couldn't parse move_speed of projectile_type:  %s.\n");
		exit(1);
	}

	/*** Despawn Delay ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	if(fscanf(fp,"%d\n",&(pr_type->despawn_delay))!=1)
	{
		fprintf(stderr,"Couldn't parse despawn_delay of projectile_type:  %s.\n");
		exit(1);
	}

	/*** Media Set ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp); 
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(pr_type->pr_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate projectile_type->pr_set_path.\n");
		exit(1);
	}
	strncpy(pr_type->pr_set_path,buffer,strlen(buffer));
	pr_type->pr_set_path[strlen(buffer)]=0;

	/*** Path ***/

	if(!(pr_type->pr_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate projectile_type->pr_type_path.\n");
		exit(1);
	}
	strncpy(pr_type->pr_type_path,path,strlen(path));
	pr_type->pr_type_path[strlen(path)]=0;

	fclose(fp);
	return pr_type;
}

void projectile_type_destroy(projectile_type *pr_type)
{
	if(!pr_type)
	{
		fprintf(stderr,"Attempt to free null projectile_type!\n");
		exit(1);
	}

	free(pr_type->pr_set_path);
	free(pr_type->pr_type_path);

	free(pr_type);

	return;
}

