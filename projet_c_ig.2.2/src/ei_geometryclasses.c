#include "ei_geometryclasses.h"
#include "ei_geometrymanager.h"
#include "ei_widget.h"
#include "ei_classes.h"
#include "ei_application.h"
#include <string.h>


ei_point_t calcul_point_ancrage(ei_rect_t* rect, ei_anchor_t *anchor){

	/*simplification expressions*/

	int x=rect->top_left.x; 
	int y=rect->top_left.y;

	int width=rect->size.width;
	int height=rect->size.height;

	ei_point_t ancrage;

	if (anchor != NULL){
                switch (*anchor) {

                case ei_anc_center :
			ancrage.x= x - width/2; 
			ancrage.y= y - height/2;
                        break;

                case ei_anc_north :
			ancrage.x= x - width/2; 
			ancrage.y= y;
                        break;

                case ei_anc_northeast :
			ancrage.x= x - width; 
			ancrage.y= y;
                        break;

                case ei_anc_east :
			ancrage.x= x - width; 
			ancrage.y= y - height/2;
                        break;

                case ei_anc_southeast :
			ancrage.x= x - width; 
			ancrage.y= y - height;
                        break;

                case ei_anc_south :
			ancrage.x= x - width/2; 
			ancrage.y= y - height;
                        break;

                case ei_anc_southwest :
			ancrage.x= x; 
			ancrage.y= y - height;
                        break;

                case ei_anc_west :
			ancrage.x= x; 
			ancrage.y= y - height/2;
                        break;

                case ei_anc_northwest :
			ancrage.x= x; 
			ancrage.y= y;
                        break;

                case ei_anc_none :
			/*cas par défault northwest*/
			ancrage.x= x; 
			ancrage.y= y;
                        break;
                }
        }else{
		/*cas par défault northwest*/
		ancrage.x= x; 
		ancrage.y= y;
	}
	return ancrage;
}


void placer_screen_location(struct ei_widget_t *widget){
        if (widget->parent == NULL) {
                /* La root_frame reste fixe */
                return;
        }
        ei_placer_param_t *placer_settings = (ei_placer_param_t *) widget->geom_params;
       
        /* -- Position -- */
        ei_rect_t new_rect;

        new_rect.top_left.x = placer_settings->x;
        new_rect.top_left.y = placer_settings->y;
        /* Pour la position relative le facteur s'applique à la largeur ou hauteur du parent */
        new_rect.top_left.x += widget->parent->content_rect->top_left.x + (placer_settings->rel_x) * widget->parent->content_rect->size.width;
        new_rect.top_left.y += widget->parent->content_rect->top_left.y + (placer_settings->rel_y) * widget->parent->content_rect->size.height;
        /* -- Taille -- */
        /*** TO DO : Utilisation de la taille relative ? Priorité face à requested size ? ***/
        new_rect.size.width = placer_settings->width;
        new_rect.size.height = placer_settings->height;
        /* if (widget->parent != NULL){ */
        /* widget->screen_location.size.width += (placer_settings->rel_width) * widget->parent->screen_location.size.width; */
        /* widget->screen_location.size.height += (placer_settings->rel_height) * widget->parent->screen_location.size.height; */
        /* } */

        /* -- Anchor -- */
        /* Mise à jour de la position : top_left */
        ei_point_t anchor_point = calcul_point_ancrage(&new_rect, &(placer_settings->anchor));
        int diff_x = anchor_point.x-widget->screen_location.top_left.x;
        int diff_y = anchor_point.y-widget->screen_location.top_left.y;
        int diff_w = new_rect.size.width-widget->screen_location.size.width;
        int diff_h = new_rect.size.height-widget->screen_location.size.height;
	widget->content_rect->top_left.x += diff_x;
        widget->content_rect->top_left.y += diff_y;
        widget->screen_location.top_left.x += diff_x;
        widget->screen_location.top_left.y += diff_y;
        widget->content_rect->size.width += diff_w;
        widget->content_rect->size.height += diff_h;
        widget->screen_location.size.width += diff_w;
        widget->screen_location.size.height += diff_h;

        /* Test de la positivité des valeurs */
        if ((widget->content_rect->size.width < 0)
           || (widget->content_rect->size.height < 0)){
                perror("Arguments de taille incohérents (valeurs finales négatives");
                exit(1);
        }
}

/* void placer_runfunc(struct ei_widget_t *widget); */
void placer_runfunc(struct ei_widget_t *widget){
        /* Calcul de la géométrie sur le widget */
        if ((widget == NULL) || (strcmp(widget->geom_params->manager->name, "placer") != 0)){
                return;
        }
        placer_screen_location(widget);
        if (strcmp(widget->wclass->name, "toplevel") == 0){
                ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
                ei_widget_t *close_button = (ei_widget_t *) widget->next_sibling;
                ei_anchor_t anch = ei_anc_center;
                ei_color_t red = {0xFF, 0x00, 0x00, 0xFF};
                ei_size_t title_size;
                hw_text_compute_size(toplevel->title,
                                     ei_default_font,
                                     &(title_size.width),
                                     &(title_size.height));
                int radius_header = title_size.height/2;
                int radius_button = title_size.height/4;
                int border_button = title_size.height/8;
                ei_size_t button_size = ei_size(radius_header, radius_header);
                ei_point_t close_button_point = ei_point(radius_header, radius_header);
                close_button_point = ei_point_add(close_button_point, widget->screen_location.top_left);
                ei_button_configure(close_button, &button_size, &red, &border_button, &radius_button, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL);
                ei_place(close_button, &anch, &(close_button_point.x), &(close_button_point.y), NULL, NULL, NULL, NULL, NULL, NULL);
        }
}


/* void placer_releasefunc(struct ei_widget_t *widget); */
void placer_releasefunc(struct ei_widget_t *widget){
        /* La disparition d'un widget nécessite de répercuter l'action sur ses fils */
        ei_widget_t *cour = widget->next_sibling;
        while (cour != NULL){
                ei_geometrymanager_unmap(cour);
                cour = cour->next_sibling;
        }
}
