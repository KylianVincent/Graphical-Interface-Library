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
        ei_color_t* couleur = calloc(1, sizeof(ei_color_t));
        char* tab = &pick_id;
        couleur->red = tab[0];
        couleur->green = tab[1];
        couleur->blue = tab[2];
        couleur->alpha = 0xFF;
        return couleur;
}

/* ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent); */
ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name, ei_widget_t* parent){
        ei_widget_t *widget = NULL;
        ei_widgetclass_t* wclass = ei_widgetclass_from_name(class_name);
        widget = (ei_widget_t *) (*wclass->allocfunc)();
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
        /* widget->requested_size et widget->screen_location->size sont initialisés à 0 */
        widget->content_rect = &(widget->screen_location);
        (*wclass->setdefaultsfunc)(widget);
        return widget;
}

/* void ei_widget_destroy(ei_widget_t* widget); */
void ei_widget_destroy(ei_widget_t* widget)
{
        if (widget == NULL) {
                return;
        }
        /* On met a jour les fils du parent et les next_sibling des fils */
        if (widget->parent != NULL) 
        {
                ei_widget_t* prec = widget->parent->children_head;
                while (prec != widget && prec->next != widget)
                {
                        prec = prec->next;
                }
                if (prec == widget) {
                        widget->parent->children_head = widget->next;
                        prec = NULL;
                }
                if (widget->parent->children_tail == widget) {
                        widget->parent->children_tail = prec;
                }
                if (prec != NULL) {
                        prec->next_sibling = widget->next_sibling;
                }
        }
        /* On parcourt les fils du widget et on libere widget */
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


/* --------------- EI_FRAME_CONFIGURE ---------------- */

void ei_frame_configure	(ei_widget_t* widget, ei_size_t* requested_size,
                         const ei_color_t* color, int* border_width,
                         ei_relief_t* relief, char** text, ei_font_t* text_font,
                         ei_color_t* text_color, ei_anchor_t* text_anchor,
                         ei_surface_t* img, ei_rect_t** img_rect,
                         ei_anchor_t* img_anchor){

        ei_frame_t* frame = (ei_frame_t*) widget;        
        /* Color */
        if (color != NULL){
                frame->color = *color;
        }
        /* Border */
        if (border_width != NULL){
                frame->border_width = *border_width;
        }
        /* Relief */
        if (relief != NULL){
                frame->relief = *relief;
        }
        /* Text */
        if ((text != NULL) && (img != NULL)){
                perror("Only one of the parameter \"text\" and \"img\" should be used");
                exit(1);
        }
        if (text != NULL){
                frame->text = *text;
                if (text_font != NULL){
                        frame->text_font = *text_font;
                }
                if (text_color != NULL){
                        frame->text_color = *text_color;
                }
                if (text_anchor != NULL){
                        frame->text_anchor = *text_anchor;
                }
        }
        /* Image */
        if (img != NULL){
                frame->img = *img;
                if (img_rect != NULL){
                        frame->img_rect = *img_rect;
                }
                if (img_anchor != NULL){
                        frame->img_anchor = *img_anchor;
                }
        }
        /* Size */
        if (requested_size == NULL){
                if (text != NULL){
                        /* Taille minimale pour l'image */
                        widget->requested_size = hw_surface_get_size(img);
                        widget->screen_location.size = widget->requested_size;
                } 
                if (img != NULL) {
                        /* Taille minimale pour le texte*/
                        hw_text_compute_size(frame->text, frame->text_font, &(widget->requested_size.height), &(widget->requested_size.width));
                        widget->screen_location.size = widget->requested_size;
                }
                /* Le cas : text != NULL et img != NULL est impossible (test antérieur) */
        } else {
                widget->requested_size = *requested_size;
                widget->screen_location.size = widget->requested_size;
        }
        
}


/* -------------- EI_BUTTON_CONFIGURE ---------------*/

void ei_button_configure(ei_widget_t*widget, ei_size_t*requested_size, 
                         const ei_color_t*color, int*border_width,
                         int*corner_radius, ei_relief_t*relief,
                         char**text, ei_font_t*text_font,
                         ei_color_t*text_color, ei_anchor_t*text_anchor,
                         ei_surface_t*img, ei_rect_t**img_rect,
                         ei_anchor_t*img_anchor, ei_callback_t*callback,
                         void**user_param)
{
        ei_frame_configure(widget, requested_size, color, border_width, relief, text, text_font, text_color, text_anchor, img, img_rect, img_anchor);
        ei_button_t *button = (ei_button_t*) widget;
        if (corner_radius != NULL) {
                button->corner_radius = *corner_radius;
        }
        if (callback != NULL) {
                button->callback = *callback;
        }
        if (user_param != NULL) {
                button->user_param = *user_param;
        }
}


/* void ei_toplevel_configure(ei_widget_t*widget, ei_size_t*requested_size,
/*                            ei_color_t*color, int*border_width, char**title, */
/*                            ei_bool_t*closable, ei_axis_set_t*resizable,   */
/*                            ei_size_t**min_size); */
