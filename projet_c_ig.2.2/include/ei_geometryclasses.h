#include <stdlib.h>
#include <stdio.h>
#include "ei_widget.h"
#include "ei_geometrymanager.h"

/* Definition du type ei_placer_param_t */
typedef struct ei_placer_param_t{
        ei_geometry_param_t          geom_params;
        ei_anchor_t                  anchor;
        int                          x;
        int                          y;
        int                          width;
        int                          height;
        float                        rel_x;
        float                        rel_y;
        float                        rel_width;
        float                        rel_height;
} ei_placer_param_t;


/* Fonctions particulières au geometry manager "placer" */
void placer_runfunc(struct ei_widget_t *widget);


void placer_releasefunc(struct ei_widget_t *widget);
