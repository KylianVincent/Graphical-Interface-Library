#include "ei_geometrymanager.h"
#include <stdlib.h>
#include <stdio.h>


/* void   ei_geometrymanager_register (ei_geometrymanager_t* geometrymanager); */


/* ei_geometrymanager_t* ei_geometrymanager_from_name (ei_geometrymanager_name_t name); */


/* void   ei_geometrymanager_unmap (ei_widget_t*  widget); */


/* void    ei_register_placer_manager  (); */


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
        if (x != NULL && y != NULL) {
                widget->screen_location.top_left.x = *x;
                widget->screen_location.top_left.y = *y;
        }
/* A compléter et modifier*/
}

