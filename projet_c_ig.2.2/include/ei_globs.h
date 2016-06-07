#include <stdlib.h>
#include <stdio.h>
#include "ei_widget.h"
#include "hw_interface.h"
#include "ei_utils.h"

/*picking*/
ei_widget_t **tab_pick;
int32_t size;
void init_picking();

/*root_frame*/
ei_surface_t main_window;
ei_surface_t main_window_picking;
ei_size_t taille_root_frame;

/*optinisation affichage*/
ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2);
ei_rect_t intersect_clipper(ei_rect_t clipper1, ei_rect_t clipper2);
