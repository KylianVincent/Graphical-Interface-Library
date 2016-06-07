#include "ei_globs.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_application.h"
#include "ei_eventmanager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void init_picking(){
	tab_pick=NULL;
	size=256;
}

ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2){
	ei_rect_t result;

	result.top_left.x = (rect1.top_left.x<=rect2.top_left.x)?
                rect1.top_left.x:rect2.top_left.x;
	result.top_left.y = (rect1.top_left.y<=rect2.top_left.y)?
                rect1.top_left.y:rect2.top_left.y;
        int max_y1 = rect1.top_left.y + rect1.size.height;
        int max_y2 = rect2.top_left.y + rect2.size.height;
        int max_x1 = rect1.top_left.x + rect1.size.width;
        int max_x2 = rect2.top_left.x + rect2.size.width;
        result.size.width = ((max_x1>=max_x2)?max_x1:max_x2)-result.top_left.x;
        result.size.height = ((max_y1>=max_y2)?max_y1:max_y2)-result.top_left.y;
	return result;
}

ei_rect_t intersect_clipper(ei_rect_t clipper1, ei_rect_t clipper2)
{
        ei_rect_t inter;
        /* On réordonne les clipper pour que x1 <= x2 */
        if (clipper1.top_left.x > clipper2.top_left.x) {
                inter = clipper1;
                clipper1 = clipper2;
                clipper2 = inter;
        }
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        x1 = clipper1.top_left.x;
        x2 = clipper2.top_left.x;
        y1 = clipper1.top_left.y;
        y2 = clipper2.top_left.y;
        w1 = clipper1.size.width;
        w2 = clipper2.size.width;
        h1 = clipper1.size.height;
        h2 = clipper2.size.height;
        /* On teste si l'intersection est vide */
        if (x1+w1<=x2 || y2+h2<=y1 || y1+h1<=y2) {
                inter.top_left.x = 0; inter.top_left.y = 0;
                inter.size.width = 0; inter.size.height = 0;
                return inter;
        }
        /* Ici, on sait qu'il y a intersection */
        if (y1 <= y2) {
                /* y2 <= y1+h1 car intersection */
                inter.top_left.y = y2;
                inter.top_left.x = x2;
                if (x1+w1 <= x2+w2) {
                        inter.size.width = x1+w1-x2;
                }
                else {
                        inter.size.width = w2;
                }
                if (y2+h2 <= y1+h1) {
                        inter.size.height = h2;
                }
                else {
                        inter.size.height = y1+h1-y2;
                }
        }
        else {
                /* y2 + h2 > y1 car intersection */
                inter.top_left.x = x2;
                inter.top_left.y = y1;
                if (x1+w1 <= x2+w2) {
                        inter.size.width = x1+w1-x2;
                }
                else {
                        inter.size.width = w2;
                }
                if (y2+h2 <= y1+h1) {
                        inter.size.height = y2+h2-y1;
                }
                else {
                        inter.size.height = h1;
                }
        }
        return inter;
}

void free_linked_rects(ei_linked_rect_t **list_rect)
{
        ei_linked_rect_t *cour = *list_rect;
        ei_linked_rect_t *temp;
        while (cour != NULL)
        {
                temp = cour;
                cour = cour->next;
                free(temp);
        }
        *list_rect = NULL;
}


ei_point_t ancrage_text_img(struct ei_widget_t* widget){

	/*simplification expressions*/

	int x=widget->content_rect->top_left.x; 
	int y=widget->content_rect->top_left.y;

	int width=widget->content_rect->size.width;
	int height=widget->content_rect->size.height;

	/*definition des variables utiles*/

	ei_point_t ancrage;
	int width_ancr=0;
	int height_ancr=0;
	
	ei_frame_t *frame = (ei_frame_t*) widget;
	ei_anchor_t anchor;

	/*différenciation cas image ou texte*/

	if (frame->text != NULL){
		
		hw_text_compute_size(frame->text,frame->text_font, &width_ancr, &height_ancr);
		anchor = frame->text_anchor;

	}else if(frame->img != NULL){
		
		height_ancr=frame->img_rect->size.height;
		width_ancr=frame->img_rect->size.width;
		anchor = frame->img_anchor;
	}

	if (frame->img != NULL ||frame->text != NULL ){

                switch (anchor) {

                case ei_anc_center :
			ancrage.x= x + (width-width_ancr)/2; 
			ancrage.y= y + (height-height_ancr)/2;
                        break;

                case ei_anc_north :
			ancrage.x= x + (width-width_ancr)/2; 
			ancrage.y= y;
                        break;

                case ei_anc_northeast :
			ancrage.x= x + width-width_ancr; 
			ancrage.y= y;
                        break;

                case ei_anc_east :
			ancrage.x= x + width-width_ancr; 
			ancrage.y= y + (height-height_ancr)/2;
                        break;

                case ei_anc_southeast :
			ancrage.x= x + width-width_ancr; 
			ancrage.y= y + height-height_ancr;
                        break;

                case ei_anc_south :
			ancrage.x= x + (width-width_ancr)/2; 
			ancrage.y= y + height-height_ancr;
                        break;

                case ei_anc_southwest :
			ancrage.x= x; 
			ancrage.y= y + height-height_ancr;
                        break;

                case ei_anc_west :
			ancrage.x= x; 
			ancrage.y= y + (height-height_ancr)/2;
                        break;

                case ei_anc_northwest :
			ancrage.x= x; 
			ancrage.y= y;
                        break;

                case ei_anc_none :
			/*cas par défault du case: northwest*/
			ancrage.x= x; 
			ancrage.y= y;
                        break;
                }
        }else{
		/*cas par défault (ancrage NULL): northwest*/
		ancrage.x= x; 
		ancrage.y= y;
	}
	return ancrage;
}



/*fonction de dessin de texte sur surface*/

void draw_texte(ei_widget_t *widget, ei_surface_t surface, ei_rect_t *clipper){
	
	ei_frame_t *frame=(ei_frame_t*) widget;
	
	if (frame->text != NULL && clipper->size.height > 0 && clipper->size.width > 0){

		ei_point_t ancrage= ancrage_text_img(widget);

		ei_draw_text(surface,
			     &(ancrage),
			     frame->text,
			     frame->text_font,
			     &(frame->text_color),
			     clipper);
	}
}


void draw_img(ei_widget_t *widget, ei_surface_t surface, ei_rect_t* clipper){
        ei_frame_t *frame=(ei_frame_t*) widget;
        if (frame->img != NULL && clipper->size.height > 0 && clipper->size.width > 0)
        {
                /* On calcule les rectangles à fournir à ei_copy_surface */
                /* Car elle doit prendre des rectangles de même taille */
                ei_rect_t dest, src;
                dest.top_left = ancrage_text_img(widget);
                src.top_left = frame->img_rect->top_left;
                int h1 = widget->content_rect->top_left.y+widget->content_rect->size.height-dest.top_left.y;
                int h2 = frame->img_rect->size.height;
                dest.size.height = (h1<=h2)?h1:h2;
                int w1 = widget->content_rect->top_left.x+widget->content_rect->size.width-dest.top_left.x;
                int w2 = frame->img_rect->size.width;
                dest.size.width = (w1<=w2)?w1:w2;
                /* On doit aussi vérifier que dest.top_left appartient a content_rect */
                if (dest.top_left.x < widget->content_rect->top_left.x) {
                        int temp = widget->content_rect->top_left.x - dest.top_left.x;
                        dest.size.width -= temp;
                        src.top_left.x += temp;
                        dest.top_left.x = widget->content_rect->top_left.x;
                }
                if (dest.top_left.y < widget->content_rect->top_left.y) {
                        int temp = widget->content_rect->top_left.y - dest.top_left.y;
                        dest.size.height -= temp;
                        src.top_left.y += temp;
                        dest.top_left.y = widget->content_rect->top_left.y;
                }
                /* On tient en compte le clipper */
                ei_rect_t temp = intersect_clipper(dest,*clipper);
                src.top_left.x += temp.top_left.x - dest.top_left.x;
                src.top_left.y += temp.top_left.y - dest.top_left.y;
                dest = temp;
                src.size = dest.size;
                int erreur=ei_copy_surface(surface, &dest, frame->img, &src, EI_TRUE);
                if (erreur){
                        perror("Problème d'insertion d'image.\n");
                        exit(1);
                }
        }
}
