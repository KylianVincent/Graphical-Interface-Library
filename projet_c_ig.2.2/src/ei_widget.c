#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_application.h"
#include "ei_utils.h"
#include "ei_globs.h"
#include "ei_eventmanager.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void alloc_tab_pick(int32_t new_size){
	
	if (size > new_size){
		perror("utilisation incorrecte de la fonction d'allocation de pick_id");
		exit(1);
	}
	ei_widget_t **tmp=tab_pick;
	if ((tab_pick=calloc(new_size, sizeof(ei_widget_t*)))==NULL){
		perror("probleme allocation tableau pick_id");
		exit(1);
	}
	if (tmp != NULL){
		for(int32_t i=0; i<size;i++){
			tab_pick[i]=tmp[i];
		}
		free(tmp);
	}	
}

ei_widget_t * 	ei_widget_pick (ei_point_t *where){
	hw_surface_lock(main_window_picking);
	uint8_t *pixel = hw_surface_get_buffer(main_window_picking);
	uint8_t *p = pixel + sizeof(uint32_t)*(where->y *taille_root_frame.width  + where->x);
	int ir;
	int ig;
	int ib;
	int ia;

	hw_surface_get_channel_indices(main_window_picking, &ir, &ig, &ib, &ia);
	hw_surface_unlock(main_window_picking);
	
	uint32_t r = *(ir+p);
	uint32_t v = *(ig+p);
	uint32_t b = *(ib+p);
	uint32_t pick_id= r + (v<<8) + (b<<16);
	if (pick_id > size){
		perror("acces incorrect");
		exit(1);
	}
	return tab_pick[pick_id];

}

ei_color_t * def_pick_color(uint32_t pick_id)
{
        /* Ici, on retourne une couleur dépendant du pick_id */
        ei_color_t* couleur = calloc(1, sizeof(ei_color_t));
        char* tab = (char*) &pick_id;
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

	/*calcul de la pick color*/
	if (tab_pick == NULL){
		alloc_tab_pick(size);
	}
	int32_t i=0;
	while (tab_pick[i] != NULL && i<size){
		i++;
	}
	if ( i >= size-1){
		alloc_tab_pick(2*size);
		size=2*size;
	}
	tab_pick[i]=widget;
        widget->pick_id = i;
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
                widget->screen_location.top_left = widget->parent->content_rect->top_left;
        }
        else 
        {
                widget->screen_location.top_left = ei_point_zero();
        }
        /* widget->requested_size et widget->screen_location->size sont initialisés à 0 */
	
	widget->content_rect = &(widget->screen_location);
        (*wclass->setdefaultsfunc)(widget);

        if (strcmp(class_name, "toplevel") == 0){
                /* On sauvegarde le pick_id de la toplevel */
                uint32_t pick_id_toplevel = widget->pick_id;
                ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
                if (toplevel->closable == EI_TRUE){
                        /* Pour les boutons associés à la toplevel aucune
                           sauvegarde de leur adresse n'est nécessaire, on se
                           servira de la structure en frères pour y accéder */
                        ei_button_t *button = (ei_button_t *) ei_widget_create("button", parent);
                        button->callback = close_toplevel;
                        button->user_param = (void *) toplevel;
                }
                if (toplevel->resizable){
                        /* On utilise une frame du même pick_id que la toplevel
                           pour gérer la zone de redimmensionnement (actif à
                           l'enfoncement d'un bouton de souris et non à son
                           relachement comme pour les boutons) */
                        ei_widget_t *resize_zone_toplevel = ei_widget_create("frame",
                                                                             parent);
                        /* Il faut libérer le pick_id alloué par la création du
                           widget frame */
                        tab_pick[resize_zone_toplevel->pick_id] = NULL;
                        resize_zone_toplevel->pick_id = pick_id_toplevel;
                        free(resize_zone_toplevel->pick_color);
                        resize_zone_toplevel->pick_color = def_pick_color(widget->pick_id);
                        
                }
        }

        return widget;
}

/* void ei_widget_destroy(ei_widget_t* widget); */
void ei_widget_destroy(ei_widget_t* widget)
{
        if (widget == NULL) {
                return;
        }
        ei_geometrymanager_unmap(widget);
        /* Si le widget à libérer est une toplevel alors il possède
           potentiellement des frères qu'il est nécessaire de libérer aussi
           (boutons de fermeture et redimensionnement */
        if (strcmp(widget->wclass->name, "toplevel") == 0){
                ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
                if (toplevel->resizable){
                        if (toplevel->closable){
                                ei_widget_destroy(widget->next_sibling->next_sibling);
                        }
                        ei_widget_destroy(widget->next_sibling);
                }
                else if (toplevel->closable){
                        ei_widget_destroy(widget->next_sibling);
                }
        }
        /* On met a jour les fils du parent et les next_sibling des fils */
        if (widget->parent != NULL)
        {
                ei_widget_t* prec = widget->parent->children_head;
                while (prec != widget && prec->next_sibling != widget)
                {
                        prec = prec->next_sibling;
                }
                if (prec == widget) {
                        widget->parent->children_head = widget->next_sibling;
                        prec = NULL;
                }
                if (widget->parent->children_tail == widget) {
                        widget->parent->children_tail = prec;
                }
                if (prec != NULL) {
                        prec->next_sibling = widget->next_sibling;
                }
        }
	/*liberation de la pick_color*/
	tab_pick[widget->pick_id]=NULL;
        /* On parcourt les fils du widget et on libere widget */
        ei_widget_t* cour = widget->children_head;
        (*widget->wclass->releasefunc)(widget);
        while (cour != NULL)
        {
                ei_widget_t* temp = cour;
                if (!strcmp(temp->wclass->name, "toplevel")){
                        if (((ei_toplevel_t *) temp)->resizable) {
                                cour = cour->next_sibling;
                                cour->parent = NULL;
                        }
                        if (((ei_toplevel_t *) temp)->closable) {
                                cour = cour->next_sibling;
                                cour->parent = NULL;
                        }
                }
                temp->parent = NULL;
                cour = cour->next_sibling;
                ei_widget_destroy(temp);
        }
}

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
        /* Relief */
        if (relief != NULL){
                frame->relief = *relief;
        }
	/* Border */
        if (border_width != NULL){
                frame->border_width = *border_width;
        }
        /* Text */
        if ((text != NULL) && (img != NULL)){
                perror("Only one of the parameter \"text\" and \"img\" should be used");
                exit(1);
        }
        if (text != NULL){
                if (frame->text != NULL) {
                        free(frame->text);
                        frame->text = NULL;
                }
                if (*text != NULL) {
                        frame->text = calloc(strlen(*text)+1, sizeof(char));
                        strcpy(frame->text,*text);
                }
        }
        if (text_font != NULL){
                frame->text_font = *text_font;
        }
        if (text_color != NULL){
                frame->text_color = *text_color;
        }
        if (text_anchor != NULL){
                frame->text_anchor = *text_anchor;
        }
        /* Image */
        if (img != NULL){
                frame->img = *img;
                if (img_rect == NULL && *img != NULL) {
                        if (frame->img_rect == NULL) {
                                frame->img_rect = calloc(1, sizeof(ei_rect_t));
                        }
                        frame->img_rect->size = hw_surface_get_size(*img);
                        frame->img_rect->top_left = ei_point_zero();
                }
                if (img_anchor == NULL && *img != NULL) {
                        frame->img_anchor = ei_anc_center;
                }
        }
        if (img_rect != NULL && frame->img != NULL){
                if (frame->img_rect == NULL) {
                        frame->img_rect = calloc(1, sizeof(ei_rect_t));
                }
                *(frame->img_rect) = **img_rect;
                /* On vérifie que le rectangle ne dépasse pas l'image */
                /* Sinon on le tronque pour éviter problèmes dans draw_img */
                ei_size_t dim_img = hw_surface_get_size(frame->img);
                if (frame->img_rect->top_left.x+frame->img_rect->size.width > dim_img.width) {
                        frame->img_rect->size.width = dim_img.width-frame->img_rect->top_left.x;
                }
                if (frame->img_rect->top_left.y+frame->img_rect->size.height > dim_img.height) {
                        frame->img_rect->size.height = dim_img.height-frame->img_rect->top_left.y;
                }
                if (frame->img_rect->top_left.x < 0) {
                        frame->img_rect->top_left.x = 0;
                }
                if (frame->img_rect->top_left.y < 0) {
                        frame->img_rect->top_left.y = 0;
                }
        }
        if (img_anchor != NULL && frame->img != NULL){
                frame->img_anchor = *img_anchor;
        }
        /* Size */
        if (requested_size == NULL && widget->parent != NULL 
            && widget->geom_params == NULL){
                /* On vérifie que le widget n'est pas la racine car 
                   il ne faut pas modifier sa taille */
                if (img != NULL && *img != NULL){
                        /* Taille minimale pour l'image */
                        if (frame->img_rect != NULL) {
                                widget->requested_size = frame->img_rect->size;
                        }
                        else {
                                widget->requested_size = hw_surface_get_size(*img);
                        }
                        /* On met à jour le screen location et le content_rect */
                        widget->requested_size.height += 2*frame->border_width;
                        widget->requested_size.width += 2*frame->border_width;
                        widget->screen_location.size = widget->requested_size;
                        (*widget->wclass->geomnotifyfunc)(widget, widget->screen_location);
                }
                if (text != NULL && *text != NULL) {
                        /* Taille minimale pour le texte*/
                        hw_text_compute_size(frame->text, frame->text_font, &(widget->requested_size.width), &(widget->requested_size.height));
                        /* On met à jour le screen location et le content_rect */
                        widget->requested_size.height += 2*frame->border_width;
                        widget->requested_size.width += 2*frame->border_width;
                        widget->screen_location.size = widget->requested_size;
                        (*widget->wclass->geomnotifyfunc)(widget, widget->screen_location);
                }
                /* Le cas : text != NULL et img != NULL est impossible (test antérieur) */
        } 
        else if (requested_size != NULL)
        {
                widget->requested_size = *requested_size;
                /* On met à jour le screen location et le content_rect */
                widget->screen_location.size.height = widget->requested_size.height + 2*frame->border_width;
                widget->screen_location.size.width = widget->requested_size.width + 2*frame->border_width;
                (*widget->wclass->geomnotifyfunc)(widget, widget->screen_location);
        }
        /* On met à jour cette partie d'écran */
        ei_app_invalidate_rect(&(widget->screen_location));
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


/* -------------- EI_TOPLEVEL_CONFIGURE ---------------*/

void ei_toplevel_configure(ei_widget_t*widget, ei_size_t*requested_size,
                           ei_color_t*color, int*border_width, char**title,
                           ei_bool_t*closable, ei_axis_set_t*resizable,
                           ei_size_t**min_size){
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        if (color != NULL){
                toplevel->color = *color;
        }
        if (border_width != NULL){
                int diff_b = *border_width - toplevel->border_width;
                toplevel->border_width = *border_width;
                widget->screen_location.top_left.x -= diff_b;
                widget->screen_location.size.width += 2*diff_b;
                widget->screen_location.size.height += diff_b;
        }
        if (title != NULL){
                if (toplevel->title != NULL) {
                        free(toplevel->title);
                        toplevel->title = NULL;
                }
                if (*title != NULL) {
                        toplevel->title = calloc(strlen(*title)+1, sizeof(char));
                        strcpy(toplevel->title,*title);
                }
                /* Mise à jour de la taille d'en-tête */
                ei_size_t title_size;
                hw_text_compute_size(toplevel->title,
                             ei_default_font,
                             &(title_size.width),
                             &(title_size.height));
                toplevel->height_header = title_size.height;
        }
        if (min_size != NULL){
                if (toplevel->min_size == NULL) {
                        toplevel->min_size = calloc(1, sizeof(ei_size_t));
                }
                *(toplevel->min_size) = **min_size;
        }
        if (requested_size != NULL){
                int diff_w = requested_size->width - widget->requested_size.width;
                int diff_h = requested_size->height - widget->requested_size.height;
                widget->requested_size = *requested_size;
                /* Mise à jour de la zone de contenu */
                widget->content_rect->size.height += diff_h;
                widget->content_rect->size.width += diff_w;
                widget->screen_location.size.height += diff_h;
                widget->screen_location.size.width += diff_w;
        }
        if (closable != NULL){
                toplevel->closable = *closable;
        }
        if (resizable != NULL){
                toplevel->resizable = *resizable;
        }
        /* On met à jour l'écran */
        ei_app_invalidate_rect(&(widget->screen_location));
}
