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

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(pr_type->damage));

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",&(pr_type->move_speed));

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp); 
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(pr_type->pr_set_path=(char*)malloc(sizeof(char)*(strlen(buffer)+1))))
	{
		fprintf(stderr,"Couldn't allocate projectile_type->pr_set_path.\n");
		exit(1);
	}
	strncpy(pr_type->pr_set_path,buffer,strlen(buffer)+1);
	pr_type->pr_set_path[strlen(buffer)]=0;

	if(!(pr_type->pr_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate projectile_type->pr_type_path.\n");
		exit(1);
	}
	strncpy(pr_type->pr_type_path,path,strlen(path)+1);
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

