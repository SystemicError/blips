/* blips_input_state.h */
/* Started 9/18/12 by Tevis Tsai */

#ifndef _BLIPS_INPUT_STATE_H
#define _BLIPS_INPUT_STATE_H

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

typedef struct blips_input_state
{
	int firing;
	double move_angle;
	double aim_angle;
	double speed;
} blips_input_state;

blips_input_state* blips_input_state_create(void);
void blips_input_state_destroy(blips_input_state *bis);

/* public functions */

#endif

