#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_globs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* --------------ENTRY-------------- */

void ei_entry_configure(struct ei_widget_t *widget, ei_color_t* color, char **text, ei_font_t *text_font, ei_color_t *text_color, int *char_max){

}

/* void entry_geomnotifyfunc (struct ei_widget_t *widget, ei_rect_t rect); */

/* void* entry_allocfunc (); */

/* void entry_releasefunc (struct ei_widget_t* widget); */

/* void entry_drawfunc(struct ei_widget_t* widget, */
/* 		     ei_surface_t		surface, */
/* 		     ei_surface_t		pick_surface, */
/* 		     ei_rect_t*		clipper); */

/* void entry_setdefaultsfunc(struct ei_widget_t* widget); */

/* void ei_entry_register_class(); */
void ei_entry_register_class()
{
        focus = NULL;
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
