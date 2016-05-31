#include "ei_geometrymanager.h"
#include "ei_geometryclasses.h"
#include "ei_types.h"
#include "ei_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ei_geometrymanager_t *geometrymanagers = NULL;

/* void   ei_geometrymanager_register (ei_geometrymanager_t* geometrymanager); */
void   ei_geometrymanager_register (ei_geometrymanager_t* geometrymanager){
        /* Pour enregistrer un geometry manager externe */
        geometrymanager->next = geometrymanagers;
        geometrymanagers = geometrymanager;
}

/* ei_geometrymanager_t* ei_geometrymanager_from_name (ei_geometrymanager_name_t name); */
ei_geometrymanager_t* ei_geometrymanager_from_name (ei_geometrymanager_name_t name){
        ei_geometrymanager_t *cour = geometrymanagers;
        while (cour != NULL){
                if (strcmp(cour->name, name) == 0){
                        return cour;
                }
		cour=cour->next;
	}
	perror("Geometry Manager inconnu");
	exit(1);
}

/* void   ei_geometrymanager_unmap (ei_widget_t*  widget); */
void   ei_geometrymanager_unmap (ei_widget_t*  widget){
        if (widget->geom_params != NULL){
                /* Release du widget */
                (*widget->geom_params->manager->releasefunc)(widget);
        
                /* Libération du champ dédié dans le widget, on ne libère pas le geometry manager lié qui peut être utilisé */
                free(widget->geom_params);

                /* Invalidation de la localisation */
                widget->geom_params = NULL;
        }

        /* Mise à 0 des composantes de screen_location */
        widget->screen_location = ei_rect_zero();
}


/* void    ei_register_placer_manager  (); */
void    ei_register_placer_manager  (){
        ei_geometrymanager_t *placer = NULL;
        placer = calloc(1, sizeof(ei_geometrymanager_t));
        strcpy(placer->name, "placer");
        placer->runfunc = placer_runfunc;
        placer->releasefunc = placer_releasefunc;

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

        /* -- Gestion de l'affiliation à un geometry manager -- */
        if (widget->geom_params == NULL){
                /* Ajout au register manager */
                if ((widget->geom_params = calloc(1, sizeof(struct ei_placer_param_t))) == NULL){
                        perror("erreur d'allocation mémoire");
                        exit(1);
                }
                widget->geom_params->manager = ei_geometrymanager_from_name("placer");
        } else if (strcmp(widget->geom_params->manager->name, "placer") != 0){
                /* Appel de la fonction de release */
                (*widget->geom_params->manager->releasefunc)(widget);
                /* Ajout au register manager */
                if ((widget->geom_params = calloc(1, sizeof(struct ei_placer_param_t))) == NULL){
                        perror("erreur d'allocation mémoire");
                        exit(1);
                }
                widget->geom_params->manager = ei_geometrymanager_from_name("placer");
        }
        /* Sinon le widget est déjà géré par le placer */
        

        /* -- Stockage/mise à jour des paramètres -- */
        ei_placer_param_t *placer_settings = (ei_placer_param_t *) widget->geom_params;
        if (anchor != NULL){
                placer_settings->anchor = *anchor;
        } else {
                placer_settings->anchor = ei_anc_northwest;
        }
        if (x != NULL){
                placer_settings->x = *x;
        } else {
                placer_settings->x = 0;
        }
        if (y != NULL){
                placer_settings->y = *y;
        } else {
                placer_settings->y = 0;
        }
        if (width != NULL){
                placer_settings->width = *width;
        } else {
                placer_settings->width = widget->requested_size.width;
        }
        if (height != NULL){
                placer_settings->height = *height;
        } else {
                placer_settings->height = widget->requested_size.height;
        }
        if (rel_x != NULL){
                placer_settings->rel_x = *rel_x;
        } else {
                placer_settings->rel_x = 0.0;
        }
        if (rel_y != NULL){
                placer_settings->rel_y = *rel_y;
        } else {
                placer_settings->rel_y = 0.0;
        }
        if (rel_width != NULL){
                placer_settings->rel_width = *rel_width;
        } else {
                placer_settings->rel_width = 0.0;
        }
        if (rel_height != NULL){
                placer_settings->rel_height = *rel_height;
        } else {
                placer_settings->rel_height = 0.0;
        }
}

