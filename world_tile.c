/* world_tile.c */
/* Started 9/18/12 by Tevis Tsai */

#include"world_tile.h"

world_tile* world_tile_create(char *path)
{
	world_tile *wt;
	FILE *fp;
	char buffer[BUFFER_SIZE];
	int i,j;

	if(!(wt=(world_tile*)malloc(sizeof(world_tile))))
	{
		fprintf(stderr,"Couldn't allocate world_tile.\n");
		exit(1);
	}
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open world_tile:  %s.\n",path);
		exit(1);
	}
printf("Reading world tile of path %s.\n",path);

	/*** Object Map ***/
printf("Reading object map.\n");

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<BLIPS_TILE_ROWS;i++)
	{
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			wt->object_strings[i][j][0]=fgetc(fp);
			wt->object_strings[i][j][1]=fgetc(fp);
		}
		fgetc(fp);
	}

	/*** Creature Barrier Map ***/
printf("Reading Cr Barrier map.\n");

	wt->creature_barriers=maze_create(BLIPS_TILE_ROWS,BLIPS_TILE_COLS);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	world_tile_parse_barriers(fp,wt->creature_barriers);

	/*** Projectile Barrier Map ***/
printf("Reading Pr Barrier map.\n");

	wt->projectile_barriers=maze_create(BLIPS_TILE_ROWS,BLIPS_TILE_COLS);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	world_tile_parse_barriers(fp,wt->projectile_barriers);

	/*** Image Map ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	for(i=0;i<BLIPS_TILE_ROWS;i++)
	{
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			wt->tile_strings[i][j][0]=fgetc(fp);
			wt->tile_strings[i][j][1]=fgetc(fp);
		}
		fgetc(fp);
	}

	/*** Background Image ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->background_image=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->background_image.\n");
		exit(1);
	}
	strncpy(wt->background_image,buffer,BUFFER_SIZE);
	wt->background_image[BUFFER_SIZE-1]=0;
printf("Found background image %s.\n",wt->background_image);

	/*** Neighboring tiles ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->north_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->north_tile.\n");
		exit(1);
	}
	strncpy(wt->north_tile,buffer,BUFFER_SIZE);
	wt->north_tile[BUFFER_SIZE-1]=0;
printf("Found north tile %s.\n",wt->north_tile);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->east_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->east_tile.\n");
		exit(1);
	}
	strncpy(wt->east_tile,buffer,BUFFER_SIZE);
	wt->east_tile[BUFFER_SIZE-1]=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->south_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->south_tile.\n");
		exit(1);
	}
	strncpy(wt->south_tile,buffer,BUFFER_SIZE);
	wt->south_tile[BUFFER_SIZE-1]=0;

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->west_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->west_tile.\n");
		exit(1);
	}
	strncpy(wt->west_tile,buffer,BUFFER_SIZE);
	wt->west_tile[BUFFER_SIZE-1]=0;

	/*** Music ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	if(!(wt->music=(char*)malloc(sizeof(char)*BUFFER_SIZE)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->music.\n");
		exit(1);
	}
	strncpy(wt->music,buffer,BUFFER_SIZE);
	wt->music[BUFFER_SIZE-1]=0;

	/*** Path ***/

	if(!(wt->path=(char*)malloc(sizeof(char)*strlen(path)+1)))
	{
		fprintf(stderr,"Couldn't allocate world_tile->path.\n");
		exit(1);
	}
	strncpy(wt->path,path,strlen(path)+1);
	wt->path[strlen(path)]=0;

	fclose(fp);
	return wt;
}

void world_tile_destroy(world_tile *wt)
{
	if(!wt)
	{
		fprintf(stderr,"Attempt to free null world_tile!\n");
		exit(1);
	}

	maze_destroy(wt->creature_barriers);
	maze_destroy(wt->projectile_barriers);

	free(wt->background_image);
	free(wt->north_tile);
	free(wt->east_tile);
	free(wt->south_tile);
	free(wt->west_tile);
	free(wt->music);
	free(wt->path);

	return;
}

void world_tile_parse_barriers(FILE *fp,maze *m)
{
	char buffer[BLIPS_TILE_COLS*2+2];  /* needs two extra chars for endline, null terminator */
	int i,j;

	for(i=0;i<BLIPS_TILE_ROWS;i++)
	{
		fgets(buffer,BLIPS_TILE_COLS*2+2,fp);
		for(j=0;j<BLIPS_TILE_COLS;j++)
		{
			switch(buffer[j*2])
			{
				case '~':
					switch(buffer[j*2+1])
					{
						case '|':
							maze_place_wall(m,i,j,MAZE_EAST);
						case '~':
							maze_place_wall(m,i,j,MAZE_NORTH);
						break;
					}
				break;
				case '_':
					switch(buffer[j*2+1])
					{
						case '|':
							maze_place_wall(m,i,j,MAZE_EAST);
						case '_':
							maze_place_wall(m,i,j,MAZE_SOUTH);
						break;
					}
				break;
				case '|':
					switch(buffer[j*2+1])
					{
						case '|':
							maze_place_wall(m,i,j,MAZE_EAST);
						case '.':
						case ',':
							maze_place_wall(m,i,j,MAZE_WEST);
						break;
						case '_':
							maze_place_wall(m,i,j,MAZE_SOUTH);
							maze_place_wall(m,i,j,MAZE_WEST);
						break;
						case '~':
							maze_place_wall(m,i,j,MAZE_NORTH);
							maze_place_wall(m,i,j,MAZE_WEST);
						break;
					}
				break;
				case '=':
					switch(buffer[j*2+1])
					{
						case '>':
							maze_place_wall(m,i,j,MAZE_EAST);
						case '=':
							maze_place_wall(m,i,j,MAZE_SOUTH);
							maze_place_wall(m,i,j,MAZE_NORTH);
						break;
					}
				break;
				case '/':
					if(buffer[j*2+1]=='\'')
					{
						maze_place_wall(m,i,j,MAZE_NORTH);
						maze_place_wall(m,i,j,MAZE_WEST);
						maze_place_wall(m,i,j,MAZE_EAST);
					}
				break;
				case '<':
					if(buffer[j*2+1]=='=')
					{
						maze_place_wall(m,i,j,MAZE_NORTH);
						maze_place_wall(m,i,j,MAZE_WEST);
						maze_place_wall(m,i,j,MAZE_SOUTH);
					}
				break;
				case '\'':
					if(buffer[j*2+1]=='/')
					{
						maze_place_wall(m,i,j,MAZE_SOUTH);
						maze_place_wall(m,i,j,MAZE_WEST);
						maze_place_wall(m,i,j,MAZE_EAST);
					}
				break;
				case '#':
					if(buffer[j*2+1]=='#')
					{
						maze_place_wall(m,i,j,MAZE_SOUTH);
						maze_place_wall(m,i,j,MAZE_NORTH);
						maze_place_wall(m,i,j,MAZE_WEST);
						maze_place_wall(m,i,j,MAZE_EAST);
					}
				break;
				case '.':
				case ',':
					if(buffer[j*2+1]=='|')
						maze_place_wall(m,i,j,MAZE_EAST);
				break;
			}
		}
	}
	return;
}

