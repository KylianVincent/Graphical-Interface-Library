#include "ei_eventmanager.h"
#include "ei_types.h"
#include "ei_application.h"
#include "ei_classes.h"


ei_linked_tag_t *tags = NULL;
ei_linked_bind_t **binds_event;

void init_binds_event()
{
        binds_event = calloc(8, sizeof(ei_linked_bind_t *));
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", button_change, (void *) NULL);
        ei_bind(ei_ev_keydown, NULL, "all", ei_app_quit_request, (void *) NULL);
}

ei_linked_bind_t **get_binds_event()
{
        return binds_event;
}

// Fonction calculant renvoyant le widget correspondant à un pixel
ei_widget_t *get_widget_of_pixel()
{
        ei_widget_t *widget = ei_app_root_widget();
        return widget->children_head->children_head;
}


// Fonction qui traite un évènement
ei_bool_t handle_event(ei_event_t* event)
{
        ei_widget_t *widget = get_widget_of_pixel();
        ei_linked_bind_t *cour = binds_event[event->type];
        ei_bool_t change = EI_FALSE;
        while (cour != NULL) {
                /** Evenements externes **/
                if (cour->widget == widget) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                /** Evenements interne **/
                if (!strcmp(cour->tag,"button") && 
                    !strcmp(widget->wclass->name, "button")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                if (!strcmp(cour->tag,"toplevel") && 
                    !strcmp(widget->wclass->name, "toplevel")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                if (!strcmp(cour->tag,"all")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
        }
        return change;
}


// Fonction traitante interne pour les boutons
ei_bool_t button_change(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_button_t* button = (ei_button_t*) widget;
        if (button->relief == 1) {
                button->relief++;
        }
        else if (button->relief == 2) {
                button->relief--;
        }
        return EI_TRUE;
}


// Fonction traitante interne pour les top_level


