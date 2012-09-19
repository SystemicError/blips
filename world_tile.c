/* world_tile.c */
/* Started 9/18/12 by Tevis Tsai */

#include"world_tile.h"

world_tile* world_tile_create(char *path)
{
	world_tile *wt;
	FILE *fp;
	char buffer[BUFFER_SIZE];
	int i,j;

	wt=(world_tile*)malloc(sizeof(world_tile));
	if(!(fp=fopen(path,"r")))
	{
		fprintf(stderr,"Couldn't open world_tile:  %s.\n",path);
		exit(1);
	}

	/*** Object Map ***/
printf("Reading (skipping) object map.\n");
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	/*UNFINISHED*/
for(i=0;i<11;i++)
fgets(buffer,BUFFER_SIZE,fp);

	/*** Creature Barrier Map ***/
printf("Reading Cr Barrier map.\n");

	wt->creature_barriers=maze_create(BLIPS_TILE_ROWS,BLIPS_TILE_COLS);
printf("Created maze.\n");
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
printf("Comment line:  %s.\n",buffer);
	world_tile_parse_barriers(fp,wt->creature_barriers);

	/*** Projectile Barrier Map ***/
printf("Reading Pr Barrier map.\n");

	wt->projectile_barriers=maze_create(BLIPS_TILE_ROWS,BLIPS_TILE_COLS);
	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
printf("Comment line:  %s.\n",buffer);
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
	wt->background_image=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->background_image,buffer);

	/*** Neighboring tiles ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	wt->north_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->north_tile,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	wt->east_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->east_tile,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	wt->south_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->south_tile,buffer);

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	wt->west_tile=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->west_tile,buffer);

	/*** Music ***/

	fgets(buffer,BUFFER_SIZE,fp);  /* comment line */
	fgets(buffer,BUFFER_SIZE,fp);
	buffer[strlen(buffer)-1]=0;  /* replace endline with null terminator */
	wt->music=(char*)malloc(sizeof(char)*BUFFER_SIZE);
	strcpy(wt->music,buffer);

	/*** Path ***/

	wt->path=(char*)malloc(sizeof(char)*strlen(path));
	strcpy(wt->path,path);

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
	/*UNFINISHED*/
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

