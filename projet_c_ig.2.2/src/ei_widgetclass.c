#include "ei_widgetclass.h"
#include "ei_classes.h"
#include <stdio.h>
#include <stdlib.h>

ei_widgetclass_t *classes = NULL;

/* void   ei_widgetclass_register  (ei_widgetclass_t* widgetclass); */
void   ei_widgetclass_register  (ei_widgetclass_t* widgetclass)
{
        
}

/* ei_widgetclass_t* ei_widgetclass_from_name (ei_widgetclass_name_t name); */
struct ei_widgetclass_t* ei_widgetclass_from_name (ei_widgetclass_name_t name)
{
        struct ei_widgetclass_t* cour = classes;
        while (cour->name[0] != name[0] && cour != NULL)
        {
                cour = cour->next;
        }
        if (cour == NULL) {
                perror("Widget introuvable");
                exit(1);
        }
        return cour;
}

/* void   ei_frame_register_class  (); */
void   ei_frame_register_class  ()
{
        ei_widgetclass_t* frame = NULL;
        frame = calloc(1, sizeof(ei_widgetclass_t));
        /* A changer, ceci est temporaire */
        frame->name[0] = 'f';
        frame->allocfunc = frame_allocfunc;
        frame->releasefunc = frame_releasefunc;
        frame->drawfunc = frame_drawfunc;
        frame->setdefaultsfunc = frame_setdefaultsfunc;
        //frame->geomnotifyfunc = frame_geomnotifyfunc;
        frame->next = classes;
        classes = frame;
}

/* void   ei_button_register_class  (); */


/* void   ei_toplevel_register_class  (); */


