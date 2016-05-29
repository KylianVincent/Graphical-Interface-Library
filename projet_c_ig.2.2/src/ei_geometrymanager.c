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

                strcpy(widget->geom_params->manager->name, "placer");
        } else if (! strcmp(widget->geom_params->manager->name, "placer")){
                /* Appel de la fonction de release */
                //*(widget->geom_params->releasefunc)();
                /* Ajout au register manager */

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

        
}

