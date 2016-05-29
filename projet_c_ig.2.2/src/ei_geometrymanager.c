#include "ei_geometrymanager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ei_geometrymanager_t *geometrymanagers = NULL;

/* void   ei_geometrymanager_register (ei_geometrymanager_t* geometrymanager); */


/* ei_geometrymanager_t* ei_geometrymanager_from_name (ei_geometrymanager_name_t name); */
ei_geometrymanager_t* ei_geometrymanager_from_name (ei_geometrymanager_name_t name){
        ei_geometrymanager_t *cour = geometrymanagers;
        while (cour != NULL){
                if (strcpy(cour->name, name)){
                        return cour;
                }
		cour=cour->next;
	}
	perror("Geometry Manager inconnu");
	exit(1);
}

/* void   ei_geometrymanager_unmap (ei_widget_t*  widget); */
void   ei_geometrymanager_unmap (ei_widget_t*  widget){
        /* Release du widget */
        (*widget->geom_params->manager->releasefunc)(widget);
        
        /* Libération du champ dédié dans le widget, on ne libère pas le geometry manager lié qui peut être utilisé */
        free(widget->geom_params);

        /* Invalidation de la localisation ?? */
        /* TO DO */

        /* Mise à 0 des composantes screen_location */
        widget->screen_location.top_left.x = 0;
        widget->screen_location.top_left.y = 0;
        widget->screen_location.size.width = 0;
        widget->screen_location.size.height = 0;
}


/* void    ei_register_placer_manager  (); */
void    ei_register_placer_manager  (){
        ei_geometrymanager_t *placer = NULL;
        placer = calloc(1, sizeof(ei_geometrymanager_t));
        strcpy(placer->name, "placer");
        /* Questions à poser sur fonctions*/
        placer->runfunc = NULL;
        placer->releasefunc = NULL;

        placer->next = geometrymanagers;
        geometrymanagers = placer;

}

/* void   ei_place   (ei_widget_t*  widget, */
/*         ei_anchor_t*  anchor, */
/*         int*   x, */
/*         int*   y, */
/*         int*   width, */
/*         int*   height, */
/*         float*   rel_x, */
/*         float*   rel_y, */
/*         float*   rel_width, */
/*         float*   rel_height); */
void   ei_place   (ei_widget_t*  widget,
        ei_anchor_t*  anchor,
        int*   x,
        int*   y,
        int*   width,
        int*   height,
        float*   rel_x,
        float*   rel_y,
        float*   rel_width,
        float*   rel_height){

        /* Gestion de l'affiliation à un geometry manager */
        if (widget->geom_params == NULL){
                /* Ajout au register manager */
                /* TO DO */

                strcpy(widget->geom_params->manager->name, "placer");
        } else if (! strcmp(widget->geom_params->manager->name, "placer")){
                /* Appel de la fonction de release */
                //*(widget->geom_params->releasefunc)(widget);
                /* Ajout au register manager */
                /* TO DO */

                strcpy(widget->geom_params->manager->name, "placer");
        } /* Sinon le widget est déjà géré par le placer */


        /* Position */
        if (x != NULL){
                widget->screen_location.top_left.x = *x;
        } else {
                widget->screen_location.top_left.x = 0;
        }
        if (y != NULL){
                widget->screen_location.top_left.y = *y;
        } else {
                widget->screen_location.top_left.y = 0;
        }

        if (rel_x != NULL){
                widget->screen_location.top_left.x += (*rel_x) * widget->parent->screen_location.top_left.x;
        }
        if (rel_y != NULL){
                widget->screen_location.top_left.y += (*rel_y) * widget->parent->screen_location.top_left.y;
        }
        

        /* Taille */
        if (width != NULL){
                widget->screen_location.size.width = *width;
        } else {
                widget->screen_location.size.width = widget->requested_size.width;
        }
        if (height != NULL){
                widget->screen_location.size.height = *height;
        } else {
                widget->screen_location.size.height = widget->requested_size.height;
        }

        if (rel_width != NULL){
                widget->screen_location.size.width += (*rel_width) * widget->parent->screen_location.size.width;
        }
        if (rel_height != NULL){
                widget->screen_location.size.height += (*rel_height) * widget->parent->screen_location.size.height;
        }

        /* Anchor */
        /* Mise à jour de la position : top_left */
        if (anchor != NULL){
                switch (*anchor) {
                        /* TO DO */
                case ei_anc_center :
                        break;
                case ei_anc_north :
                        break;
                case ei_anc_northeast :
                        break;
                case ei_anc_east :
                        break;
                case ei_anc_southeast :
                        break;
                case ei_anc_south :
                        break;
                case ei_anc_southwest :
                        break;
                case ei_anc_west :
                        break;
                case ei_anc_northwest :
                        /* Rien à faire, calcul initial valide */
                        break;
                case ei_anc_none :
                        break;
                }
        }
        /* Default to : ei_anc_northwest, le point est donc bien placé par défaut*/
                
}

