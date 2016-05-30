#include "ei_widgetclass.h"
#include "ei_classes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ei_widgetclass_t *classes = NULL;

/* void   ei_widgetclass_register  (ei_widgetclass_t* widgetclass); */
void   ei_widgetclass_register  (ei_widgetclass_t* widgetclass){
        /* Enregistrement d'une classe externe */
        widgetclass->next = classes;
        classes = widgetclass;
}

/* ei_widgetclass_t* ei_widgetclass_from_name (ei_widgetclass_name_t name); */
struct ei_widgetclass_t* ei_widgetclass_from_name (ei_widgetclass_name_t name){
	ei_widgetclass_t *cour=classes;
	while (cour != NULL){
                if (strcmp(cour->name, name) == 0){
                        return cour;
                }
		cour=cour->next;
	}
	perror("Classe inconnue");
	exit(1);
}

/* void   ei_frame_register_class  (); */
void   ei_frame_register_class  ()
{
        ei_widgetclass_t* frame = NULL;
        frame = calloc(1, sizeof(ei_widgetclass_t));
        if (frame == NULL){
                perror("Impossible d'allouer la mémoire dédiée à la classe frame");
                exit(1);
        }
        strcpy(frame->name, "frame");
        frame->allocfunc = frame_allocfunc;
        frame->releasefunc = frame_releasefunc;
        frame->drawfunc = frame_drawfunc;
        frame->setdefaultsfunc = frame_setdefaultsfunc;
        //frame->geomnotifyfunc = frame_geomnotifyfunc;
        frame->next = classes;
        classes = frame;
}

/* void   ei_button_register_class  (); */
void ei_button_register_class()
{
        ei_widgetclass_t* button = NULL;
        button = calloc(1, sizeof(ei_widgetclass_t));
        if (button == NULL) {
                perror("Impossible d'allouer la mémoire dédiée à la classe button.\n");
                exit(1);
        }
        strcpy(button->name, "button");
        button->allocfunc = button_allocfunc;
        button->releasefunc = button_releasefunc;
        button->drawfunc = button_drawfunc;
        button->setdefaultsfunc = button_setdefaultsfunc;
        //button->geomnotifyfunc = button_geomnotifyfunc;
        button->next = classes;
        classes = button;
}

/* void   ei_toplevel_register_class  (); */


