#include "ei_geometrymanager.h"
#include "ei_geometryclasses.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_application.h"
#include "ei_classes.h"
#include "ei_globs.h"
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
        
                /* Libération du champ dédié dans le widget, on ne libère pas
                   le geometry manager lié qui peut être utilisé */
                free(widget->geom_params);

                /* Invalidation de la localisation */
                widget->geom_params = NULL;
        }

        /* Mise à 0 des composantes de screen_location */
        //widget->screen_location = ei_rect_zero();

        /* Unmap des boutons inclus si le widget est une toplevel */
        if (strcmp(widget->wclass->name, "toplevel") == 0){
                ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
                if (toplevel->closable){
                        if (toplevel->resizable) {
                                ei_geometrymanager_unmap(widget->next_sibling->next_sibling);
                        }
                        ei_geometrymanager_unmap(widget->next_sibling);
                }
                else if (toplevel->resizable){
                        ei_geometrymanager_unmap(widget->next_sibling);
                }
        }
        
        /* On unmap les fils du widget */
        ei_widget_t* cour = widget->children_head;
        while (cour != NULL) {
                ei_geometrymanager_unmap(cour);
                cour = cour->next_sibling;
        }
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
        if (widget->geom_params != NULL && strcmp(widget->geom_params->manager->name, "placer") != 0){
                /* Appel de la fonction de release */
                ei_geometrymanager_unmap(widget);
        }
        
        ei_placer_param_t *placer_settings;
        if (widget->geom_params == NULL){
                /* Ajout au register manager */
                if ((widget->geom_params = calloc(1, sizeof(struct ei_placer_param_t))) == NULL){
                        perror("Erreur d'allocation mémoire");
                        exit(1);
                }
                widget->geom_params->manager = ei_geometrymanager_from_name("placer");

                placer_settings = (ei_placer_param_t *) widget->geom_params;
                /* Mise aux valeurs par défaut */
                placer_settings->anchor = ei_anc_northwest;
                placer_settings->x = 0;
                placer_settings->y = 0;
                placer_settings->width = widget->requested_size.width;
                placer_settings->height = widget->requested_size.height;
                placer_settings->rel_x = 0.0;
                placer_settings->rel_y = 0.0;
                placer_settings->rel_width = 0.0;
                placer_settings->rel_height = 0.0;
        }
        /* Sinon le widget est déjà géré par le placer */

        placer_settings = (ei_placer_param_t *) widget->geom_params;

        /* -- Mise à jour des paramètres -- */
        if (anchor != NULL){
                placer_settings->anchor = *anchor;
        }
        if (x != NULL){
                placer_settings->x = *x;
        }
        if (y != NULL){
                placer_settings->y = *y;
        }
        if (width != NULL){
                placer_settings->width = *width;
        }
        if (height != NULL){
                placer_settings->height = *height;
        }
        if (rel_x != NULL){
                placer_settings->rel_x = *rel_x;
        }
        if (rel_y != NULL){
                placer_settings->rel_y = *rel_y;
        }
        if (rel_width != NULL){
                placer_settings->rel_width = *rel_width;
        }
        if (rel_height != NULL){
                placer_settings->rel_height = *rel_height;
        }
        ei_rect_t old_rect = widget->screen_location;
        /* Calcul de la géométrie du widget et de ses fils*/
        (*widget->geom_params->manager->runfunc)(widget);

        ei_rect_t maj_rect = union_rect(widget->screen_location, old_rect);
        ei_widget_t *root = ei_app_root_widget();
        maj_rect = intersect_clipper(maj_rect, *(root->content_rect));
        ei_app_invalidate_rect(&maj_rect);
}

