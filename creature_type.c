/* creature_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"creature_type.h"

creature_type* creature_type_new(char *path);
{
	FILE *fp;
	creature_type *cr_type;
	char buffer[1024];

	cr_type=(creature_type*)malloc(sizeof(creature_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open creature_type file:  %s.\n",path);
		exit(1);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",cr_type->max_health);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	cr_type->pr_type_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(cr_type->pr_type_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",cr_type->move_speed);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",cr_type->fire_delay);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	cr_type->ai_type_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(cr_type->ai_type_path,buffer);

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%s\n",buffer);
	cr_type->cr_set_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(cr_type->cr_set_path,buffer);

	cr_type->cr_type_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(cr_type->cr_type_path,buffer);

	return cr_type;
}

void creature_type_destroy(creature_type *cr_type)
{
	if(!cr_type)
	{
		fprintf(stderr,"Attempt to free null creature_type!\n");
		exit(1);
	}

	free(cr_type->pr_set_path);
	free(cr_type->ai_set_path);
	free(cr_type->cr_set_path);
	free(cr_type->cr_type_path);

	free(cr_type);

	return;
}

