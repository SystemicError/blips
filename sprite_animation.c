/* sprite_animation.c */
/* Started 1/20/10 by Tevis Tsai */

#include"sprite_animation.h"

sprite_animation* sprite_animation_new(void)
{
	sprite_animation *sa;
	sa=(sprite_animation*)malloc(sizeof(sprite_animation));
	sa->num_frames=0;
	sa->count=0;
	sa->frames=0;
	return sa;
}

void sprite_animation_destroy(sprite_animation *sa)
{
	int i;
	if(!sa)
	{
		fprintf(stderr,"Attempt to free null sprite animation!\n");
		exit(1);
	}
	for(i=0;i<sa->num_frames;i++)
		cairo_surface_destroy(sa->frames[i]);
	if(sa->frames)
		free(sa->frames);
	return;
}

SDL_Surface* sprite_animation_cycle(sprite_animation *sa)
{
	cairo_surface_t *s;
	if(sa->num_frames==0)
	{
		fprintf(stderr,"Attempt to cycle empty sprite animation!  Quiting.\n");
		exit(1);
	}
	s=sa->frames[sa->count++];
	if(sa->count>=sa->num_frames)
		sa->count=0;
	return s;
}

void sprite_animation_add_frame(sprite_animation *sa,const char *path)
{
/*printf("Loading sprite animation frame from path %s.\n",complete_path);
*/
	sa->frames=(cairo_surface_t**)realloc(sa->frames,sizeof(cairo_surface_t*)*(sa->num_frames+1));
	sa->frames[sa->num_frames]=cairo_image_surface_create_from_png(path);
	sa->num_frames++;

	/* error checking */
	switch(cairo_surface_status(sa->frames[sa->num_frames-1]))
	{
/* UNFINISHED */
		case CAIRO_STATUS_NO_MEMORY:
		case CAIRO_STATUS_FILE_NOT_FOUND:
		case CAIRO_STATUS_READ_ERROR:
			fprintf(stderr,"Error loading sprite animation frame:  %s\n",complete_path);
			exit(1);
		break;
	}
	return;
}

