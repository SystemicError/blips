/* string_map.c */
/* Started 9/21/12 by Tevis Tsai */

/* There's no reason a more general map couldn't be made from this using function pointers to replace the comparison operation. */

#include"string_map.h"

string_map* string_map_create(void)
{
	string_map *smap;

	if(!(smap=(string_map*)malloc(sizeof(string_map))))
	{
		fprintf(stderr,"Couldn't allocate string_map.\n");
		exit(1);
	}

	smap->size=0;
	smap->pointers=0;
	smap->strings=0;

	return smap;
}

void string_map_destroy(string_map *smap)
{
	if(!smap)
	{
		fprintf(stderr,"Attempt to destroy null string_map!\n");
		exit(1);
	}

	if(smap->size)
	{
		free(smap->pointers);
		free(smap->strings);
	}

	free(smap);

	return;
}

/* public functions */

void string_map_add(string_map *smap,char *string,void *ptr)
{
	if(!(smap->pointers=(void**)realloc(smap->pointers,sizeof(void*)*(smap->size+1))))
	{
		fprintf(stderr,"Couldn't allocate string_map->pointers.\n");
		exit(1);
	}
	if(!(smap->strings=(char**)realloc(smap->strings,sizeof(char*)*(smap->size+1))))
	{
		fprintf(stderr,"Couldn't allocate string_map->strings.\n");
		exit(1);
	}

	smap->pointers[smap->size]=ptr;
	smap->strings[smap->size]=string;

	smap->size++;

	string_map_sort(smap);

	return;
}

void string_map_string_to_pointer(string_map *smap,char *string,void **ret)
{
	/*** This function is a generic binary search algorithm. ***/

	int upper,lower,middle,comparison;

	if(smap->size<=0)
	{
		(*ret)=0;
		return;
	}

	upper=smap->size;
	lower=0;
	middle=(lower+upper)/2;

	while(comparison=strcmp(string,smap->strings[middle]))
	{
		if(comparison>0)  /* input>candidate */
			lower=middle+1;
		else
			upper=middle;

		if(lower==upper)
		{
			(*ret)=0;
			return;
		}

		middle=(lower+upper)/2;
	}
	(*ret)=smap->pointers[middle];

	return;
}

/* private functions */

void string_map_sort(string_map *smap)
{
	int i,j;
	void *ptmp;
	char *chtmp;

	for(i=0;i<smap->size-1;i++)
		for(j=i;j<smap->size;j++)
			if(strcmp(smap->strings[i],smap->strings[j])>0)
			{
				/* switch 'em */
				ptmp=smap->pointers[i];
				smap->pointers[i]=smap->pointers[j];
				smap->pointers[j]=ptmp;

				chtmp=smap->strings[i];
				smap->strings[i]=smap->strings[j];
				smap->strings[j]=chtmp;
			}
	return;
}

