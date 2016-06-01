#include "ei_geometryclasses.h"
#include "ei_geometrymanager.h"
#include "ei_classes.h"
#include "ei_application.h"
#include <string.h>


ei_point_t calcul_point_ancrage(struct ei_widget_t* widget, ei_anchor_t *anchor){

	/*simplification expressions*/

	int x=widget->screen_location.top_left.x; 
	int y=widget->screen_location.top_left.y;

	int width=(widget->screen_location.size).width;
	int height=(widget->screen_location.size).height;

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
        ei_placer_param_t *placer_settings = (ei_placer_param_t *) widget->geom_params;
        

        /* -- Position -- */
	int bord_x =  widget->content_rect->top_left.x - widget->screen_location.top_left.x;
	int bord_y =  widget->content_rect->top_left.y - widget->screen_location.top_left.y;

        widget->screen_location.top_left.x = placer_settings->x;
        widget->screen_location.top_left.y = placer_settings->y;
        /* Pour la position relative le facteur s'applique à la largeur ou hauteur du parent */
        if (widget->parent != NULL){
                widget->screen_location.top_left.x += widget->parent->content_rect->top_left.x + (placer_settings->rel_x) * widget->parent->screen_location.size.width;
                widget->screen_location.top_left.y += widget->parent->content_rect->top_left.y + (placer_settings->rel_y) * widget->parent->screen_location.size.height;

        }
	widget->content_rect->top_left.x = widget->screen_location.top_left.x + bord_x;
	widget->content_rect->top_left.y = widget->screen_location.top_left.y + bord_y;
        /* -- Taille -- */
        /*** TO DO : Utilisation de la taille relative ? Priorité face à requested size ? ***/
        widget->screen_location.size.width = placer_settings->width;
        widget->screen_location.size.height = placer_settings->height;
        widget->content_rect->size.width = placer_settings->width - 2*bord_x;
        widget->content_rect->size.height = placer_settings->height- 2*bord_y;
        /* if (widget->parent != NULL){ */
        /* widget->screen_location.size.width += (placer_settings->rel_width) * widget->parent->screen_location.size.width; */
        /* widget->screen_location.size.height += (placer_settings->rel_height) * widget->parent->screen_location.size.height; */
        /* } */

        /* -- Anchor -- */
        /* Mise à jour de la position : top_left */
        ei_point_t anchor_point = calcul_point_ancrage(widget, &(placer_settings->anchor));
        widget->screen_location.top_left.x = anchor_point.x;
        widget->screen_location.top_left.y = anchor_point.y;
	widget->content_rect->top_left.x = anchor_point.x + bord_x;
        widget->content_rect->top_left.y = anchor_point.y + bord_y;

        /* Test de la positivité des valeurs */
        if ((widget->content_rect->top_left.x < 0)
           || (widget->content_rect->top_left.y < 0)
           || (widget->content_rect->size.width < 0)
           || (widget->content_rect->size.height < 0)){
                perror("Arguments de taille et position incohérents (valeurs finales négatives");
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
