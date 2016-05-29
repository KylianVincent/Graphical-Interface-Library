#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
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
                widget->screen_location.top_left = widget->parent->screen_location.top_left;
        }
        else 
        {
                widget->screen_location.top_left = ei_point_zero();
        }
        widget->content_rect = &(widget->screen_location);
        return widget;
}

/* void ei_widget_destroy(ei_widget_t* widget); */
void ei_widget_destroy(ei_widget_t* widget)
{
        ei_widget_t* cour = widget->children_head;
        (*widget->wclass->releasefunc)(widget);
        while (cour != NULL)
        {
                ei_widget_t* temp = cour;
                cour = cour->next_sibling;
                ei_widget_destroy(temp);
        }
}


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

        ei_frame_t* frame = (ei_frame_t*) widget;        
        
        /* Color */
        if (color == NULL){
                if (/*** Non définie ***/ true){
                        frame->color = ei_default_background_color;
                }
        } else {
                frame->color = *color;
        }
        
        /* Border */
        if (border_width == NULL){
                if (/*** Non définie ***/ true){
                        frame->border_width = 0;
                }
        } else {
                frame->border_width = *border_width;
        }

        /* Relief */
        if (relief == NULL){
                if (/*** Non définie ***/ true){
                        frame->relief = ei_relief_none;
                }
        } else {
                frame->relief = *relief;
        }

        /* Text */
        if ((text != NULL) && (img != NULL)){
                perror("Only one of the parameter \"text\" and \"img\" should be used");
                exit(1);
        }
        if (text == NULL){
                if (/*** Non définie ***/ true){
                        frame->text = NULL;
                }
        } else {
                frame->text = *text;
                
                /* Text -- Font */
                if (text_font == NULL){
                        if (/*** Non définie ***/ true){
                                frame->text_font = ei_default_font;
                        }
                } else {
                        frame->text_font = *text_font;
                }
                /* Text -- Color */
                if (text_color == NULL){
                        if (/*** Non définie ***/ true){
                                frame->text_color = ei_font_default_color;
                        }
                } else {
                        frame->text_color = *text_color;
                }
                /* Text -- Anchor */
                if (text_anchor == NULL){
                        if (/*** Non définie ***/ true){
                                frame->text_anchor = ei_anc_center;
                        }
                } else {
                        frame->text_anchor = *text_anchor;
                }
        }

        /* Image */
        if (img == NULL){
                if (/*** Non définie ***/ true){
                        frame->img = NULL;
                }
        } else {
                frame->img = *img;
                
                /* Img -- Rect */
                if (img_rect == NULL){
                        if (/*** Non définie ***/ true){
                                frame->img_rect = NULL;
                        }
                } else {
                        frame->img_rect = *img_rect;
                }
                /* Img -- Anchor */
                if (img_anchor == NULL){
                        if (/*** Non définie ***/ true){
                                frame->img_anchor = ei_anc_center;
                        }
                } else {
                        frame->img_anchor = *img_anchor;
                }
        }


        
        /* Size */
        if (requested_size == NULL){
                if (/*** Non définie ***/ true){
                        if (text == NULL){
                                if (img == NULL){
                                        (widget->requested_size).height = 0;
                                        (widget->requested_size).width = 0;
                                } else {
                                        /* Taille minimale pour l'image */
                                        widget->requested_size = hw_surface_get_size(img);
                                }
                        } else if (img == NULL) {
                                /* Taille minimale pour le texte*/
                                hw_text_compute_size(frame->text, frame->text_font, &(widget->requested_size.height), &(widget->requested_size.width));
                        }
                        /* Le cas : text != NULL et img != NULL est impossible (test antérieur) */
                }
        } else {
                widget->requested_size = *requested_size;
                widget->screen_location.size = widget->requested_size;
        }
        
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
