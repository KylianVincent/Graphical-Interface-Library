#include "ei_widget.h"
#include "ei_widgetclass.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint32_t free_pick = 0;

ei_color_t * def_pick_color(uint32_t pick_id)
{
        /* Ici, on retourne une couleur dépendant du pick_id */
        /* N'a pas trop d'importance mais bien pour le débug */
        ei_color_t* couleur = calloc(1, sizeof(ei_color_t));
        couleur->red = (char) 7*pick_id;
        couleur->green = (char) 19*pick_id;
        couleur->blue = (char) 41*pick_id;
        couleur->alpha = 0xFF;
        return couleur;
}

/* ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent); */
ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent){
        ei_widget_t *widget = NULL;
        ei_widgetclass_t* wclass = ei_widgetclass_from_name(class_name);
        widget = (ei_widget_t *) wclass->allocfunc();
        widget->wclass = wclass;
        widget->pick_id = free_pick;
        free_pick++;
        widget->pick_color = def_pick_color(widget->pick_id);
        if (parent != NULL) {
                widget->parent = parent;
                if (parent->children_tail != NULL) {
                        parent->children_tail->next_sibling = widget;
                        parent->children_tail = widget;
                }
                else {
                        parent->children_head = widget;
                        parent->children_tail = widget;
                }
        }
        return widget;
}

/* void ei_widget_destroy(ei_widget_t* widget); */


/* ei_widget_t* ei_widget_pick(ei_point_t*	where); */


/* void ei_frame_configure	(ei_widget_t* widget, ei_size_t* requested_size,  */
/*                          const ei_color_t* color, int* border_width,  */
/*                          ei_relief_t* relief, char** text, ei_font_t* text_font, */
/*                          ei_color_t* text_color, ei_anchor_t* text_anchor,  */
/*                          ei_surface_t* img, ei_rect_t** img_rect,  */
/*                          ei_anchor_t* img_anchor); */
void ei_frame_configure	(ei_widget_t* widget, ei_size_t* requested_size,
                         const ei_color_t* color, int* border_width,
                         ei_relief_t* relief, char** text, ei_font_t* text_font,
                         ei_color_t* text_color, ei_anchor_t* text_anchor,
                         ei_surface_t* img, ei_rect_t** img_rect,
                         ei_anchor_t* img_anchor){
        /* Corps vide */
}

/* void ei_button_configure(ei_widget_t*widget, ei_size_t*requested_size, */
/*                          const ei_color_t*color, int*border_width, */
/*                          int*corner_radius, ei_relief_t*relief, */
/*                          char**text, ei_font_t*text_font, */
/*                          ei_color_t*text_color, ei_anchor_t*text_anchor, */
/*                          ei_surface_t*img, ei_rect_t**img_rect, */
/*                          ei_anchor_t*img_anchor, ei_callback_t*callback, */
/*                          void**user_param); */


/* void ei_toplevel_configure(ei_widget_t*widget, ei_size_t*requested_size, */
/*                            ei_color_t*color, int*border_width, char**title, */
/*                            ei_bool_t*closable, ei_axis_set_t*resizable,   */
/*                            ei_size_t**min_size); */
