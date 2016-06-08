#include <stdlib.h>
#include <stdio.h>
#include "ei_widget.h"
#include "hw_interface.h"
#include "ei_utils.h"

/*-------VARIABLES GLOBALES-------*/
/*root_frame*/
ei_surface_t main_window;
ei_surface_t main_window_picking;
ei_size_t taille_root_frame;

/* Taille de la zone de redimensionnement */
int min_resize_zone;

/*picking*/
ei_widget_t **tab_pick;
int32_t size;
void init_picking();

/*-------FONCTIONS UTILES-------*/
/*optimisation affichage*/
ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2);

ei_rect_t intersect_clipper(ei_rect_t clipper1, ei_rect_t clipper2);

void free_linked_rects(ei_linked_rect_t **list_rect);

/*fonctions auxiliaires*/
ei_point_t ancrage_text_img(struct ei_widget_t* widget);
void draw_texte(ei_widget_t *widget, ei_surface_t surface, ei_rect_t *clipper);
void draw_img(ei_widget_t *widget, ei_surface_t surface, ei_rect_t* clipper);
