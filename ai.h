/* ai.h */
/* Started 9/19/12 by Tevis Tsai */

#ifndef _AI_H
#define _AI_H 1

#ifndef _STDIO_H
#include<stdio.h>
#endif

#ifndef _STDLIB_H
#include<stdlib.h>
#endif

#ifndef _DEFS_H
#include"defs.h"
#endif

#ifndef _AI_TYPE_H
#include"ai_type.h"
#endif

typedef struct ai
{
	ai_type *type;
} ai;

#endif

