#include "ei_geometryclasses.h"
#include "ei_geometrymanager.h"
#include "ei_classes.h"
#include "ei_application.h"
#include <string.h>



void placer_screen_location(struct ei_widget_t *widget){
        ei_placer_param_t *placer_settings = (ei_placer_param_t *) widget->geom_params;
        

        /* -- Position -- */
        widget->screen_location.top_left.x = placer_settings->x;
        widget->screen_location.top_left.y = placer_settings->y;
        /* Pour la position relative le facteur s'applique à la largeur ou hauteur du parent */
        widget->screen_location.top_left.x += widget->parent->screen_location.top_left.x + (placer_settings->rel_x) * widget->parent->screen_location.size.width;
        widget->screen_location.top_left.y += widget->parent->screen_location.top_left.y + (placer_settings->rel_y) * widget->parent->screen_location.size.height;

        /* -- Taille -- */
        /*** TO DO : Utilisation de la taille relative ? Priorité face à requested size ? ***/
        widget->screen_location.size.width = placer_settings->width;
        widget->screen_location.size.height = placer_settings->height;
        /* if (widget->parent != NULL){ */
        /* widget->screen_location.size.width += (placer_settings->rel_width) * widget->parent->screen_location.size.width; */
        /* widget->screen_location.size.height += (placer_settings->rel_height) * widget->parent->screen_location.size.height; */
        /* } */

        /* -- Anchor -- */
        /* Mise à jour de la position : top_left */
        ei_point_t anchor_point = calcul_point_ancrage(widget, &(placer_settings->anchor));
        widget->screen_location.top_left.x = anchor_point.x;
        widget->screen_location.top_left.y = anchor_point.y;

        /* Test de la positivité des valeurs */
        if ((widget->screen_location.top_left.x < 0)
           || (widget->screen_location.top_left.y < 0)
           || (widget->screen_location.size.width < 0)
           || (widget->screen_location.size.height < 0)){
                perror("Arguments de taille et position incohérents (valeurs finales négatives");
                exit(1);
        }
}

/* void placer_runfunc(struct ei_widget_t *widget); */
void placer_runfunc(struct ei_widget_t *widget){
        /* Calcul de la géométrie sur le widget ainsi que ses frères et fils */
        if ((widget == NULL) || (! strcmp(widget->geom_params->manager->name, "placer"))){
                return;
        }
        placer_screen_location(widget);
}


/* void placer_releasefunc(struct ei_widget_t *widget); */
void placer_releasefunc(struct ei_widget_t *widget){
        /* La disparition d'un widget nécessite de répercuter l'action sur ses fils */
}
