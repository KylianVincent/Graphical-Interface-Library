#include <stdlib.h>
#include <stdio.h>
#include "ei_widget.h"
#include "hw_interface.h"
#include "ei_utils.h"

/*root_frame*/
ei_surface_t main_window;
ei_surface_t main_window_picking;
ei_size_t taille_root_frame;

/*picking*/
ei_widget_t **tab_pick;
int32_t size;
void init_picking();

/*optinisation affichage*/
ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2);

ei_rect_t intersect_clipper(ei_rect_t clipper1, ei_rect_t clipper2);

void free_linked_rects(ei_linked_rect_t **list_rect);
