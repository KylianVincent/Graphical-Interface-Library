#include <stdlib.h>
#include <stdio.h>
#include "hw_interface.h"
#include "ei_classes.h"

ei_frame_t* frame_allocfunc(){

	struct ei_frame_t *frame = calloc(1, sizeof(struct ei_frame_t));
	if (frame != NULL){
		return frame;	
	}
	perror("Erreur allocation frame. \n");
	exit(1);
}


void frame_releasefunc(struct ei_widget_t* widget){

	ei_frame_t* frame  = (ei_frame_t*) widget; /*on caste le widget pour acceder à toute la zone mémoire*/
	
	free(&(frame->requested_size));
	free(&(frame->color));
	free(&(frame->border_width));
	free(&(frame->relief));

	free(frame->text);
/*on ne libère pas la police (elle est utilisée par toutes les autres fonctions)*/
	free(&(frame->text_color));
	free(&(frame->text_anchor));
	free(frame->img);

	free(frame->img_rect);

	free(&(frame->img_anchor));
}

void frame_drawfunc(struct ei_widget_t*	frame,
			   ei_surface_t		surface,
			   ei_surface_t		pick_surface,
			   ei_rect_t*		clipper){

	int x=frame->screen_location.top_left.x; /*Origine du dessin*/
	int y=frame->screen_location.top_left.y;

	ei_linked_point_t points[4];
	int coords[]={x, y, 
		      x+(frame->requested_size).width, y,
		      x+(frame->requested_size).width,y+(frame->requested_size).width,
		      x, y+(frame->requested_size).width};
		      
	for (int i = 0; i < 8; i+=2) {
		points[i/2].point.x	= coords[i];
		points[i/2].point.y	= coords[i + 1];

		if (i < 6)
			points[i/2].next	= &(points[i/2+1]);
		else
			points[i/2].next	= NULL;	
	}

	hw_surface_lock(surface);
	hw_surface_lock(pick_surface);
	ei_draw_polygon(surface, points, ((ei_frame_t*) frame)->color, clipper);
	ei_draw_polygon(pick_surface, points, *(frame->pick_color), clipper);

	hw_surface_unlock(surface);
	hw_surface_unlock(pick_surface);

	hw_surface_update_rects(surface, NULL);
	hw_surface_update_rects(pick_surface, NULL);

}

void frame_setdefaultsfunc(struct ei_widget_t* widget){
	ei_frame_t* frame  = (ei_frame_t*) widget; /*on caste le widget pour acceder à toute la zone mémoire*/
	/*frame->requested_size=(0,0);*/
	frame->color= ei_default_background_color;
	frame->border_width=0;
	frame->relief=ei_relief_none;
	frame->text=NULL;
	frame->text_font=ei_default_font;
	frame->text_color=ei_font_default_color;
	frame->text_anchor=ei_anc_center;
	frame->img=NULL;
	frame->img_rect=NULL;
	frame->img_anchor=ei_anc_center;
}
