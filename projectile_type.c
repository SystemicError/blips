/* projectile_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"projectile_type.h"

	int damage;
	int move_speed;
	char *pr_set_path;
	char *pr_type_path;

projectile_type* projectile_type_create(char *path)
{
	FILE *fp;
	projectile_type *pr_type;
	char buffer[BUFFER_SIZE];

	pr_type=(projectile_type*)malloc(sizeof(projectile_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open projectile_type file:  %s.\n",path);
		exit(1);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",pr_type->damage);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fscanf(fp,"%d\n",pr_type->move_speed);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp); 
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	pr_type->pr_set_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(pr_type->pr_set_path,buffer);

	pr_type->pr_type_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(pr_type->pr_type_path,buffer);

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

