#include "ei_geometryclasses.h"
#include "ei_geometrymanager.h"
#include "ei_widget.h"
#include "ei_classes.h"
#include "ei_application.h"
#include <string.h>


ei_point_t calcul_point_ancrage(struct ei_widget_t* widget, ei_anchor_t *anchor){

	/*simplification expressions*/

	int width=(widget->screen_location.size).width;
	int height=(widget->screen_location.size).height;

	ei_point_t ancrage;

	if (anchor != NULL){
                switch (*anchor) {

                case ei_anc_center :
			ancrage.x= - width/2; 
			ancrage.y= - height/2;
                        break;

                case ei_anc_north :
			ancrage.x= - width/2; 
			ancrage.y= 0;
                        break;

                case ei_anc_northeast :
			ancrage.x= - width; 
			ancrage.y= 0;
                        break;

                case ei_anc_east :
			ancrage.x= - width; 
			ancrage.y= - height/2;
                        break;

                case ei_anc_southeast :
			ancrage.x= - width; 
			ancrage.y= - height;
                        break;

                case ei_anc_south :
			ancrage.x= - width/2; 
			ancrage.y= - height;
                        break;

                case ei_anc_southwest :
			ancrage.x= 0; 
			ancrage.y= - height;
                        break;

                case ei_anc_west :
			ancrage.x= 0; 
			ancrage.y= - height/2;
                        break;

                case ei_anc_northwest :
			ancrage.x= 0; 
			ancrage.y= 0;
                        break;

                case ei_anc_none :
			/*cas par défault northwest*/
			ancrage.x= 0; 
			ancrage.y= 0;
                        break;
                }
        }else{
		/*cas par défault northwest*/
		ancrage.x= 0; 
		ancrage.y= 0;
	}
	return ancrage;
}


void placer_screen_location(struct ei_widget_t *widget){
        ei_placer_param_t *placer_settings = (ei_placer_param_t *) widget->geom_params;
        

        /* -- Position -- */
	int move_x = widget->screen_location.top_left.x - placer_settings->x;
	int move_y = widget->screen_location.top_left.y - placer_settings->y;
        /* Pour la position relative le facteur s'applique à la largeur ou hauteur du parent */
        if (widget->parent != NULL){
                move_x -= widget->parent->content_rect->top_left.x + (placer_settings->rel_x) * widget->parent->screen_location.size.width;
                move_y -= widget->parent->content_rect->top_left.y + (placer_settings->rel_y) * widget->parent->screen_location.size.height;
        }
        /* -- Anchor -- */
        /* Mise à jour de la position */
        ei_point_t anchor_point = calcul_point_ancrage(widget, &(placer_settings->anchor));
        move_x += anchor_point.x;
        move_y += anchor_point.y;

        
        /* Mise à jour des valeurs */
	widget->screen_location.top_left.x -= move_x;
	widget->screen_location.top_left.y -= move_y;
	widget->content_rect->top_left.x -= move_x;
	widget->content_rect->top_left.y -= move_y;


        /* -- Taille -- */
	int resize_width = widget->screen_location.size.width - placer_settings->width;
	int resize_height = widget->screen_location.size.height - placer_settings->height;
        /*** TO DO : Utilisation de la taille relative ? Priorité face à requested size ? ***/
        widget->screen_location.size.width -= resize_width;
        widget->screen_location.size.height -= resize_height;
        
        widget->content_rect->size.width -= resize_width;
        widget->content_rect->size.height -= resize_height;

        /* if (widget->parent != NULL){ */
        /* widget->screen_location.size.width += (placer_settings->rel_width) * widget->parent->screen_location.size.width; */
        /* widget->screen_location.size.height += (placer_settings->rel_height) * widget->parent->screen_location.size.height; */
        /* } */


        /* Test de la positivité des valeurs */
        if ((widget->content_rect->top_left.x < 0)
           || (widget->content_rect->top_left.y < 0)
           || (widget->content_rect->size.width < 0)
           || (widget->content_rect->size.height < 0)){
                perror("Arguments de taille et position incohérents (valeurs finales négatives)");
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
                ei_point_t close_button_point = ei_point(radius_header, radius_header);
                close_button_point = ei_point_add(close_button_point, widget->screen_location.top_left);
                ei_button_configure(close_button, NULL, &red, NULL, NULL, NULL, NULL, NULL, NULL, &anch, NULL, NULL, NULL, NULL, NULL);
                ei_place(close_button, NULL, &(close_button_point.x), &(close_button_point.y), NULL, NULL, NULL, NULL, NULL, NULL);
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
