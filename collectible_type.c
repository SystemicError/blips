/* collectible_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"collectible_type.h"

collectible_type* collectible_type_create(char *path)
{
	FILE *fp;
	collectible_type *co_type;
	char buffer[1024];

	co_type=(collectible_type*)malloc(sizeof(collectible_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open collectible_type file:  %s.\n",path);
		exit(1);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */

	fscanf(fp,"%s\n",buffer);
	co_type->co_set_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(co_type->co_set_path,buffer);

	co_type->co_type_path=(char*)malloc(sizeof(char)*strlen(buffer));
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

