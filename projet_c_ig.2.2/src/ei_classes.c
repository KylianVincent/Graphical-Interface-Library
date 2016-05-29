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

        if (widget != NULL)
        {
                free(widget->pick_color);
                ei_frame_t* frame  = (ei_frame_t*) widget; /*on caste le widget pour acceder à toute la zone mémoire*/
                free(frame);
        }
	/* free(&(frame->color)); */
	/* free(&(frame->border_width)); */
	/* free(&(frame->relief)); */

	/* free(frame->text); */

	/* free(&(frame->text_color)); */
	/* free(&(frame->text_anchor)); */
	/* free(frame->img); */

	/* free(frame->img_rect); */

	/* free(&(frame->img_anchor)); */
}

void frame_drawfunc(struct ei_widget_t*	frame,
		    ei_surface_t		surface,
		    ei_surface_t		pick_surface,
		    ei_rect_t*		clipper){

	int x=frame->screen_location.top_left.x; /*Origine du dessin*/
	int y=frame->screen_location.top_left.y;

	/*simplification expressions*/

	int b= ((ei_frame_t*)frame)->border_width; 

	int width=(frame->screen_location.size).width;
	int height=(frame->screen_location.size).height;

	ei_linked_point_t points[4];

/*création du cadre principal en teant compte des bordures*/
	int coords[]={x+b, y+b, 
		      x+width-b, y+b,
		      x+width-b, y+height-b,
		      x+b, y+height-b};
		      
	for (int i = 0; i < 8; i+=2) {
		points[i/2].point.x	= coords[i];
		points[i/2].point.y	= coords[i + 1];

		if (i < 6){
			points[i/2].next	= &(points[i/2+1]);
		}else{
			points[i/2].next	= NULL;	
		}

		/*blocage de la surface pour le dessin des triangles*/
		hw_surface_lock(surface);
		hw_surface_lock(pick_surface);
	}

/*traitement du relief*/

	if ((((ei_frame_t*)frame)->relief ==1 ||((ei_frame_t*)frame)->relief ==2)  && b!=0){

		ei_linked_point_t points_triangle1[4];
		ei_linked_point_t points_triangle2[4];
		
		/*création des deux tringles créant l'imptression de relief*/
		int coords_triangle1[]={x, y, 
					x+width, y,
					x, y+height};

		int coords_triangle2[]={x+width, y,
					x+width, y+height,
					x, y+height};

		/*création de la liste chainée de points*/
		for (int i = 0; i < 6; i+=2) {

			points_triangle1[i/2].point.x	= coords_triangle1[i];
			points_triangle1[i/2].point.y	= coords_triangle1[i + 1];

			points_triangle2[i/2].point.x	= coords_triangle2[i];
			points_triangle2[i/2].point.y	= coords_triangle2[i + 1];

			if (i < 4){
				points_triangle1[i/2].next	= &(points_triangle1[i/2+1]);
				points_triangle2[i/2].next	= &(points_triangle2[i/2+1]);
			}else{
				points_triangle1[i/2].next	= NULL;	
				points_triangle2[i/2].next	= NULL;	
			

			}		
		}
		ei_color_t color_triangle1=((ei_frame_t*)frame)->color;
		ei_color_t color_triangle2=((ei_frame_t*)frame)->color;
		/*delta permet de modifier uniformément la couleyr des triangles*/
		unsigned char delta;
		
		if (((ei_frame_t*)frame)->relief ==1){

			delta =100; /*cas above*/
		}else{
			delta=-100; /*cas under*/
		}

		color_triangle1.red +=delta %255;
		color_triangle1.green += delta %255;
		color_triangle1.blue += delta %255;

		color_triangle2.red -=delta %255;
		color_triangle2.green -=delta %255;
		color_triangle2.blue -= delta %255;

		/*dessin des deux triangles*/
		ei_draw_polygon(surface, points_triangle1, color_triangle1, clipper);
		ei_draw_polygon(surface, points_triangle2, color_triangle2, clipper);

	}


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
