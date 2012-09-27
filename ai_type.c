/* ai_type.c */
/* Started 9/17/12 by Tevis Tsai. */
/* The only type without an associated media set! */

#include"ai_type.h"

ai_type* ai_type_create(char *path)
{
	ai_type *ait;
	FILE *fp;
	char buffer[BUFFER_SIZE];

	if(!(ait=(ai_type*)malloc(sizeof(ai_type))))
	{
		fprintf(stderr,"Couldn't allocate ai_type.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open ai_type file:  %s.\n",path);
		exit(1);
	}

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	if(!strcmp(buffer,"dodge"))
		ait->move_goal=AI_DODGE;
	else if(!strcmp(buffer,"flee"))
		ait->move_goal=AI_FLEE;
	else if(!strcmp(buffer,"close"))
		ait->move_goal=AI_CLOSE;
	else if(!strcmp(buffer,"wander"))
		ait->move_goal=AI_WANDER;
	else if(!strcmp(buffer,"patrol_ns"))
		ait->move_goal=AI_PATROL_NS;
	else if(!strcmp(buffer,"patrol_ew"))
		ait->move_goal=AI_PATROL_EW;

	
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	if(!strcmp(buffer,"face"))
		ait->aim_goal=AI_FACE;
	else if(!strcmp(buffer,"spin"))
		ait->aim_goal=AI_SPIN;
	else if(!strcmp(buffer,"lag_face"))
		ait->aim_goal=AI_LAG_FACE;
	else if(!strcmp(buffer,"lead_face"))
		ait->aim_goal=AI_LEAD_FACE;
	else if(!strcmp(buffer,"as_velocity"))
		ait->aim_goal=AI_AS_VELOCITY;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */

	if(!strcmp(buffer,"spam"))
		ait->fire_goal=AI_SPAM;
	else if(!strcmp(buffer,"sight_first"))
		ait->fire_goal=AI_SIGHT_FIRST;
	else if(!strcmp(buffer,"spurt"))
		ait->fire_goal=AI_SPURT;

	if(!(ait->ai_type_path=(char*)malloc(sizeof(char)*(strlen(path)+1))))
	{
		fprintf(stderr,"Couldn't allocate ai_type->ai_type_path.\n");
		exit(1);
	}
	strncpy(ait->ai_type_path,path,strlen(path)+1);
	ait->ai_type_path[strlen(path)]=0;

	fclose(fp);
	return ait;
}

void ai_type_destroy(ai_type *ait)
{
	if(!ait)
	{
		fprintf(stderr,"Attempt to free null ai_type!\n");
		exit(1);
	}

	if(ait->ai_type_path)
		free(ait->ai_type_path);
	free(ait);
	return;
}

