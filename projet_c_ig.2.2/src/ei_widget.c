#include "ei_widget.h"
#include "ei_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/* ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent); */
ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent){
        /* Corps vide */
        return NULL;
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

        ei_frame_t* frame = (ei_frame_t) widget;        
        
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
                                        (frame->requested_size).height = 0;
                                        (frame->requested_size).width = 0;
                                } else {
                                        /* Taille minimale pour l'image */
                                        frame->requested_size = hw_surface_get_size(img);
                                }
                        } else if (img == NULL) {
                                /* Taille minimale pour le texte*/
                                hw_text_compute_size(frame->text, frame->text_font, &(frame->requested_size.height), &(frame->requested_size.width));
                        }
                        /* Le cas : text != NULL et img != NULL est impossible (test antérieur) */
                } else {
                        frame->requested_size = *requested_size;
                }
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
