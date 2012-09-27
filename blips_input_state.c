/* blips_input_state.c */
/* Started 9/18/12 by Tevis Tsai */

#include"blips_input_state.h"

blips_input_state* blips_input_state_create(void)
{
	blips_input_state *bis;
	if(!(bis=(blips_input_state*)malloc(sizeof(blips_input_state))))
	{
		fprintf(stderr,"Couldn't allocate blips_input_state.\n");
		exit(1);
	}
	return bis;
}

void blips_input_state_destroy(blips_input_state *bis)
{
	if(!bis)
	{
		fprintf(stderr,"Attempt to free null blips_input_state!\n");
		exit(1);
	}
	free(bis);
	return;
}

/* public functions */

