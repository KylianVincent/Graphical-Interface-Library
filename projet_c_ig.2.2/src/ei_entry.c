#include "ei_entry.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_widgetclass.h"
#include "ei_globs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* --------------ENTRY-------------- */
/* void ei_entry_configure(struct ei_widget_t *widget, ei_size_t *requested_size, */
/*                         ei_color_t* color, char **text, ei_font_t *text_font, */
/*                         ei_color_t *text_color, int *char_max); */
void ei_entry_configure(struct ei_widget_t *widget, ei_size_t *requested_size,
                        ei_color_t* color, char **text, ei_font_t *text_font,
                        ei_color_t *text_color, int *char_max)
{
        ei_entry_t *entry = (ei_entry_t *) widget;
        
        if (color != NULL){
                entry->color = *color;
        }
        if (text != NULL){
                entry->text = *text;
        }
        if (text_font != NULL){
                entry->text_font = *text_font;
        }
        if (text_color != NULL){
                entry->text_color = *text_color;
        }
        if (char_max != NULL){
                entry->char_max = *char_max;
        }
        
        /* Taille */
        if (requested_size != NULL){
                widget->requested_size = *requested_size;
        } else if (text_font != NULL){
                /* On doit alors faire concorder la hauteur du widget avec la
                   taille de la fonte */
                        int temp;
                        hw_text_compute_size("A", *text_font, &(temp),
                                             &(widget->requested_size.height));
        }
        
        /* Calcul du screen_location avec 1 pixel de délimitation autour du
         champ de saisie */
        widget->screen_location.size.width = widget->requested_size.width + 2;
        widget->screen_location.size.height = widget->requested_size.height + 2;
        (*widget->wclass->geomnotifyfunc)(widget, widget->screen_location);
}


/* void entry_geomnotifyfunc (struct ei_widget_t *widget, ei_rect_t rect); */
void entry_geomnotifyfunc (struct ei_widget_t *widget, ei_rect_t rect)
{
        if (widget != NULL){
                if (widget->content_rect == &(widget->screen_location)){
                        /* Le content_rect pointe vers la screen_location, il
                           faut allouer la place nécessaire au content_rect */
                        widget->content_rect = calloc(1, sizeof(ei_rect_t));
                }
        widget->content_rect->top_left.x = widget->screen_location.top_left.x + 1;
        widget->content_rect->top_left.x = widget->screen_location.top_left.y + 1;
        widget->content_rect->size.width = widget->screen_location.size.width - 2;
        widget->content_rect->size.height = widget->screen_location.size.height - 2;
        }
}


/* void entry_allocfunc ();*/
void* entry_allocfunc ()
{
        ei_entry_t *entry = calloc(1, sizeof(ei_entry_t));
	if (entry == NULL){	
                perror("Erreur allocation entry. \n");
                exit(1);
	}
        return (void*) entry;
}


/* void entry_releasefunc (struct ei_widget_t* widget); */
void entry_releasefunc (struct ei_widget_t* widget)
{
if (widget != NULL){
                free(widget->pick_color);
                ei_entry_t* entry = (ei_entry_t*) widget;
                if (entry->text != NULL){
                        free(entry->text);
                }
		if (widget->content_rect != &(widget->screen_location)){
			free(widget->content_rect);
		}
                free(entry);
        }
}


/* void entry_drawfunc(struct ei_widget_t* widget, */
/* 		     ei_surface_t		surface, */
/* 		     ei_surface_t		pick_surface, */
/* 		     ei_rect_t*		clipper); */
void entry_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper)
{
        ei_entry_t *entry = (ei_entry_t *) widget;
        ei_linked_point_t *exterior = rounded_frame(widget->screen_location, 
						    0, 0);
        ei_linked_point_t *interior = rounded_frame(*(widget->content_rect), 
						    0, 0);
        ei_color_t outline = {0x00, 0x00, 0x00, 0x00};

        /* Tracé des surfaces */
	hw_surface_lock(surface);
	hw_surface_lock(pick_surface);
        ei_draw_polygon(surface, exterior, outline, clipper);
        ei_draw_polygon(surface, interior, entry->color, clipper);
        ei_draw_polygon(pick_surface, exterior, *(widget->pick_color), clipper);
        
        /* Affichage du texte */
        if (entry->text != NULL) {
                ei_draw_text(surface,
                             &(widget->content_rect->top_left),
                             entry->text,
                             entry->text_font,
                             &(entry->text_color),
                             clipper);
        }
        
        hw_surface_unlock(surface);
        hw_surface_unlock(pick_surface);

        /* On libère les listes de points */
        free_linked_point(exterior);
        free_linked_point(interior);
}


/* void entry_setdefaultsfunc(struct ei_widget_t* widget); */
void entry_setdefaultsfunc(struct ei_widget_t* widget)
{
        ei_entry_t* entry = (ei_entry_t*) widget;
        ei_color_t white = {0xFF, 0xFF, 0xFF, 0xFF};
        entry->color = white;
        entry->text = NULL;
        entry->text_font = ei_default_font;
        entry->text_color = ei_font_default_color;
        entry->char_max = 0;
        
        /* Taille */
        /* Pour la hauteur on prend la taille d'une majuscule dans la fonte
           condidérée */
        int temp;
        hw_text_compute_size("A", entry->text_font, &(temp),
                             &(widget->requested_size.height));
        widget->requested_size.width = 100;
}


/* void ei_entry_register_class(); */
void ei_entry_register_class()
{
        focus = NULL;
        capital = EI_FALSE;
        ei_widgetclass_t *entry = NULL;
        entry = calloc(1, sizeof(ei_entry_t));
        if (entry == NULL){
                perror("Impossible d'allouer la mémoire dédiée à la classe entry");
                exit(1);
        }
        strcpy(entry->name, "entry");
        entry->allocfunc = entry_allocfunc;
        entry->releasefunc = entry_releasefunc;
        entry->drawfunc = entry_drawfunc;
        entry->setdefaultsfunc = entry_setdefaultsfunc;
        entry->geomnotifyfunc = entry_geomnotifyfunc;
        ei_widgetclass_register(entry);
}
