#include "ei_classes.h"
#include <math.h>

/* ----------FRAME------------*/

void* frame_allocfunc(){

	struct ei_frame_t *frame = calloc(1, sizeof(struct ei_frame_t));
	if (frame != NULL){
		return (void*) frame;	
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


/* --------------BUTTON-------------- */

void* button_allocfunc ()
{
        ei_button_t* button = calloc(1, sizeof(ei_button_t));
        if (button == NULL) {
                perror("Impossible d'allouer l'espace pour un bouton.\n");
                exit(1);
        }
        return (void*) button;
}

void button_releasefunc (struct ei_widget_t* widget)
{
        free(widget->pick_color);
        ei_button_t* button = (ei_button_t*) widget;
        free(button);
}

/* Trace un arc de points avec c:centre / r:rayon / a1:angle de début / a2:angle de fin */
/* Requiert a1 < a2 pour tracer quelque chose, et tail != NULL */
/* Insert l'arc à la suite de tail, et place tail à la fin */
void arc(ei_point_t c, float r, float a1, float a2, ei_linked_point_t** tail)
{
        const float pas = 2.0;
        *tail = calloc(1, sizeof(ei_linked_point_t));
        pts->point.x = r*cos(a1)+c.x;
        pts->point.y = r*sin(a1)+c.y;
        ei_linked_point_t* cour = *tail;
        int k = 1;
        while (a1+(float) k*pas/r < a2) {
                cour->next = calloc(1, sizeof(ei_linked_point_t));
                cour = cour->next;
                cour->point.x = r*cos(a1+(float)k*pas/r)+c.x;
                cour->point.y = r*sin(a1+(float)k*pas/r)+c.y;
                k++;
        }
        cour->next = calloc(1, sizeof(ei_linked_point_t));
        cour = cour->next;
        cour->point.x = r*cos(a2)+c.x;
        cour->point.y = r*sin(a2)+c.y;
        *tail = cour;
}

/* Trace un bouton complet ou seulement la partie haut ou basse */
/* mode = 0 : complet  //  mode = -1 : basse  //  mode = 1 : haute  */
ei_linked_point_t* rounded_frame(ei_rect_t rect, int radius, int8_t mode)
{

}

void button_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper)
{

}


void button_setdefaultsfunc(struct ei_widget_t* widget)
{
        ei_button_t* button  = (ei_button_t*) widget; 
	button->color = ei_default_background_color;
	button->border_width = k_default_button_border_width;
        button->corner_radius = k_default_button_corner_radius;
	button->relief = ei_relief_none;
	button->text = NULL;
	button->text_font = ei_default_font;
	button->text_color = ei_font_default_color;
	button->text_anchor = ei_anc_center;
	button->img = NULL;
	button->img_rect = NULL;
	button->img_anchor = ei_anc_center;
        button->callback = NULL;
        button->user_param = NULL;
}
