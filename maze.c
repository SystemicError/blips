/* maze.c */
/* Separated from main.c on 8/16/10 by Tevis Tsai */
/* Repurposed for Squawk Box on 6/8/12 */

#include"maze.h"

maze* maze_create(int rows,int columns)
{
	maze *m;
	int i;

	if(!(m=(maze*)malloc(sizeof(maze))))
	{
		fprintf(stderr,"Couldn't allocate maze.\n");
		exit(1);
	}
	m->rows=rows;
	m->columns=columns;

	/* check for null mazes */
	if(!(rows*columns))
	{
printf("Created zero-dimensional maze.\n");
		m->tiles=0;
		return m;
	}

	/* create maze tiles */
	/* this is a 2d array, [rows*columns][4] in size.  The second index is a list of links to other tiles. */
	if(!(m->tiles=(int**)malloc(sizeof(int*)*rows*columns)))
	{
		fprintf(stderr,"Couldn't allocate maze->tiles.\n");
		exit(1);
	}
	for(i=0;i<rows*columns;i++)
		if(!(m->tiles[i]=(int*)malloc(sizeof(int)*4)))
		{
			fprintf(stderr,"Couldn't allocate maze->tiles[%d].\n",i);
			exit(1);
		}

	/* start with maximal linkage */
	for(i=0;i<rows*columns;i++)
	{
		m->tiles[i][MAZE_NORTH]=1;
		m->tiles[i][MAZE_EAST]=1;
		m->tiles[i][MAZE_SOUTH]=1;
		m->tiles[i][MAZE_WEST]=1;
	}
	return m;
}

void maze_generate(maze *m)
{
	int cell, direction, cell2, direction2, is_maximal;
	/* consider a random link, and delete it if it would not partition the maze */
	do
	{
		/* select random link */
		do
		{
			cell=rand()%(m->rows*m->columns); /* this could actually rest outside the loop, since no cell will have all directions cut, but then we increase the likelyhood of three-walled cells being tested */
			direction=rand()%4;
		}while(!m->tiles[cell][direction]);
			/* for convenience, record the other end of the link, too */
		maze_get_opposing_link(m,cell,direction,&cell2,&direction2);
		/* will this partition the maze? */
			/* if not, delete it */
		if(!maze_partitioned_by_wall(m,cell,direction,cell2,direction2))
			m->tiles[cell][direction]=m->tiles[cell2][direction2]=0;
	}while(!maze_is_maximal(m));
	return;
}


void maze_destroy(maze *m)
{
	int i;
	for(i=0;i<m->rows*m->columns;i++)
		free(m->tiles[i]);
	if(m->tiles)
		free(m->tiles);
	if(m)
		free(m);
	else
	{
		fprintf(stderr,"Attempt to destroy null maze!\n");
		exit(1);
	}

	return;
}

void maze_place_wall(maze *m,int r,int c,int direction)
{
	int cell, cell2, direction2;
	cell=r*m->columns+c;
	m->tiles[cell][direction]=0;
	maze_get_opposing_link(m,cell,direction,&cell2,&direction2);

	if(direction2==-1)  /* opposing link could be outside maze */
		return;

	m->tiles[cell2][direction2]=0;
	return;
}

int maze_contains_wall(maze *m,int r,int c,int direction)
{
	if(r<0)
		return maze_contains_wall(m,0,c,direction);
	if(r>m->rows-1)
		return maze_contains_wall(m,m->rows-1,c,direction);
	if(c<0)
		return maze_contains_wall(m,r,0,direction);
	if(c>m->columns-1)
		return maze_contains_wall(m,r,m->columns-1,direction);
	return !(m->tiles[r*m->columns+c][direction]);
}

void maze_print(maze *m)
{
	int i,j;
	for(i=0;i<m->rows*m->columns;i++)
	{
		printf("*");
		if(m->tiles[i][MAZE_EAST] && m->tiles[i+1][MAZE_WEST])
			printf("--");
		else
			printf("  ");
		if((i+1)%m->columns==0)
		{
			printf("\n");
			for(j=0;j<m->columns;j++)
			{
				if(m->tiles[i-m->columns+1+j][MAZE_SOUTH] && m->tiles[i+1+j][MAZE_NORTH])
					printf("|");
				else
					printf(" ");
				printf("  ");
			}
			printf("\n");
		}
	}
	return;
}

int maze_partitioned_by_wall(maze *m,int cell,int direction,int cell2,int direction2)
{
	m->tiles[cell][direction]=m->tiles[cell2][direction2]=0;
	if(minimum_distance_between(m,cell,cell2)==-1) /* if we just partitioned */
	{
		m->tiles[cell][direction]=m->tiles[cell2][direction2]=1;
		return 1;
	}
	m->tiles[cell][direction]=m->tiles[cell2][direction2]=1;
	return 0;
}

void maze_get_opposing_link(maze *m,int cell,int direction,int *cell2,int *direction2)
{
	/* sets direction2 to -1 if outside maze */
	switch(direction)
	{
		case MAZE_NORTH:
			*cell2=cell-m->columns;
			if(cell<m->columns)
				*direction2=-1;
			else
				*direction2=MAZE_SOUTH;
		break;
		case MAZE_SOUTH:
			*cell2=cell+m->columns;
			if(cell>=m->columns*(m->rows-1))
				*direction2=-1;
			else
				*direction2=MAZE_NORTH;
		break;
		case MAZE_EAST:
			*cell2=cell+1;
			if(cell%m->columns==m->columns-1)
				*direction2=-1;
			else
				*direction2=MAZE_WEST;
		break;
		case MAZE_WEST:
			*cell2=cell-1;
			if(cell%m->columns==0)
				*direction2=-1;
			else
				*direction2=MAZE_EAST;
		break;
	}
	return;
}

int maze_is_maximal(maze *m)
{
	int i,j,k,n;
	for(i=0;i<m->rows*m->columns;i++)
		for(j=0;j<4;j++)
			if(m->tiles[i][j])
			{
				maze_get_opposing_link(m,i,j,&k,&n);
				if(!maze_partitioned_by_wall(m,i,j,k,n))
					return 0;
			}
	return 1;
}

/* differs from the partitions_maze method because the number of steps is important */
int minimum_distance_between(maze *m,int start,int finish)
{
	int i,j,k;
	int cell,direction;
	int *hits;

	if(!(hits=(int*)malloc(sizeof(int)*m->rows*m->columns)))
	{
		fprintf(stderr,"Couldn't allocate hits in maze_minimum_distance_between.\n");
		exit(1);
	}
	/* each entry in hits corresponds to the number of steps to get there */
	for(i=0;i<m->rows*m->columns;i++)
		hits[i]=-1;
	hits[start]=0;

	for(i=0;i<m->rows*m->columns;i++)  /* even the slowest traversal can't take longer than the number of cells */
	{
		/* grow our distance map */
		for(j=0;j<m->rows*m->columns;j++)
			if(hits[j]==i)
				for(k=0;k<4;k++)
					if(m->tiles[j][k])
					{
						maze_get_opposing_link(m,j,k,&cell,&direction); /* direction not used */
						if(hits[cell]==-1)
							hits[cell]=i+1;
					}

		/* are we there, yet? */
		if(hits[finish]!=-1)
		{
			free(hits);
			return i+1;
		}
	}
	free(hits);
	return -1;
}

