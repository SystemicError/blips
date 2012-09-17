/* breakable_type.c */
/* Started 9/17/12 by Tevis Tsai. */

#include"breakable_type.h"

breakable_type* breakable_type_new(char *path)
{
	breakable_type *br_type;
	FILE *fp;
	char buffer[1024];

	br_type=(breakable_type*)malloc(sizeof(breakable_type));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open breakable_type file:  %s.\n",path);
		exit(1);
	}

	fscanf(fp,"%s\n",buffer);  /* comment line */
	fscanf(fp,"%d\n",&(br_type->toughness));
	fscanf(fp,"%s\n",buffer);  /* comment line */

	fscanf(fp,"%s\n",buffer);
	br_type->br_set_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(br_set->br_type_path,buffer);

	br_type->br_type_path=(char*)malloc(sizeof(char)*strlen(buffer));
	strcpy(br_type->br_type_path,buffer);

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

