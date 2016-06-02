#include "ei_eventmanager.h"
#include "ei_types.h"
#include "ei_application.h"
#include "ei_classes.h"
#include "ei_geometryclasses.h"
#include "ei_application.h"
#include "ei_widget.h"


ei_linked_tag_t *tags = NULL;
ei_linked_bind_t **binds_event;
/* Utile pour le déplacement de fenetre */
ei_point_t last_pos;

void init_binds_event()
{
        binds_event = calloc(8, sizeof(ei_linked_bind_t *));
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        ei_bind(ei_ev_keydown, NULL, "all", ei_app_quit_request, NULL);
}

ei_linked_bind_t **get_binds_event()
{
        return binds_event;
}

// Fonction calculant renvoyant le widget correspondant à un pixel
ei_widget_t *get_widget_of_pixel(ei_point_t where)
{
	return ei_widget_pick(&where);	
}


// Fonction qui traite un évènement
ei_bool_t handle_event(ei_event_t* event)
{
        ei_widget_t *widget = get_widget_of_pixel(event->param.mouse.where);
        ei_linked_bind_t *cour = binds_event[event->type];
        ei_bool_t change = EI_FALSE;
        while (cour != NULL) {
                ei_linked_bind_t *next = cour->next;
                /** Evenements externes **/
                if (cour->widget == widget) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                /** Evenements interne **/
                else if (!strcmp(cour->tag,"button") && 
                    !strcmp(widget->wclass->name, "button")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                else if (!strcmp(cour->tag,"toplevel") && 
                    !strcmp(widget->wclass->name, "toplevel")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                else if (!strcmp(cour->tag,"all")) {
                        (*cour->callback)(widget, event, cour->user_param);
                        change = EI_TRUE;
                }
                cour = next;
        }
        return change;
}


// Fonction traitante interne pour les boutons
ei_bool_t click_button(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ((ei_button_t*) widget)->relief++;
        ei_unbind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ei_bind(ei_ev_mouse_move, NULL, "all", click_moveout, (void *) widget);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        return EI_TRUE;
}

ei_bool_t click_moveout(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_widget_t *ancien_widget = (ei_widget_t *) user_param;
        if (widget != ancien_widget && EI_FALSE) {
                ((ei_button_t*) ancien_widget)->relief--;
                ei_unbind(ei_ev_mouse_move, NULL, "all", click_moveout, user_param);
                ei_unbind(ei_ev_mouse_buttonup, ancien_widget, NULL, unclick_button, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "button", click_movein, NULL);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", unclick, NULL);
                return EI_TRUE;
        }
        return EI_FALSE;
}

ei_bool_t click_movein(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "button", click_movein, NULL);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick, NULL);
        ei_bind(ei_ev_mouse_move, NULL, "all", click_moveout, (void *) widget);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        return EI_TRUE;
}

ei_bool_t unclick(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "button", click_movein, NULL);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        return EI_FALSE;
}

ei_bool_t unclick_button(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "all", click_moveout, widget);
        ei_unbind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ((ei_button_t*) widget)->relief--;
        return EI_TRUE;
}


// Fonction traitante interne pour les top_level

ei_bool_t click_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        if (event->param.mouse.where.y < widget->screen_location.top_left.y + ((ei_toplevel_t *) widget)->height_header) {
                last_pos = event->param.mouse.where;
                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", move_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, widget, NULL, unclick_toplevel, NULL);
        }
        return EI_FALSE;
}

ei_bool_t move_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_widget_t *toplevel = (ei_widget_t *) user_param;
        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        ((ei_placer_param_t *) toplevel->geom_params)->x += diff_x;
        ((ei_placer_param_t *) toplevel->geom_params)->y += diff_y;
        last_pos = event->param.mouse.where;
        return EI_TRUE;
}

ei_bool_t unclick_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "all", move_toplevel, widget);
        ei_unbind(ei_ev_mouse_buttonup, widget, NULL, unclick_toplevel, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        return EI_FALSE;
}
