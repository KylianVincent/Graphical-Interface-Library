#include "ei_eventmanager.h"
#include "ei_types.h"
#include "ei_application.h"
#include "ei_classes.h"
#include "ei_geometryclasses.h"
#include "ei_application.h"
#include "ei_widget.h"


ei_linked_bind_t **binds_event;
/* Utile pour le déplacement et redimmensionnement de fenetre */
ei_point_t last_pos;

void init_binds_event()
{
        binds_event = calloc(8, sizeof(ei_linked_bind_t *));
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        ei_bind(ei_ev_keydown, NULL, "all", escape, NULL);
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
                        change = change || (*cour->callback)(widget, event, cour->user_param);
                }
                /** Evenements interne **/
                else if (cour->tag != NULL && !strcmp(cour->tag,widget->wclass->name)) {
                        change = change || (*cour->callback)(widget, event, cour->user_param);
                }
                else if (cour->tag != NULL && !strcmp(cour->tag,"all")) {
                        change = change || (*cour->callback)(widget, event, cour->user_param);
                }
                cour = next;
        }
        return change;
}

// Fonction qui quitte lorqu'on appuye sur échap
ei_bool_t escape(ei_widget_t *widget, ei_event_t* event, void *user_param)
{
        if (event->type == ei_ev_keydown && event->param.key.key_sym == SDLK_ESCAPE) {
                ei_app_quit_request();
        }
        return EI_FALSE;
}

// Fonction traitante interne pour les boutons

void change_relief(ei_widget_t *widget) 
{
        int rel = ((ei_button_t*) widget)->relief;
        if (rel == 1) {
                ((ei_button_t*) widget)->relief++;
        } else if (rel == 2) {
                ((ei_button_t*) widget)->relief--;
        }
}
ei_bool_t click_button(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        change_relief(widget);
        ei_unbind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ei_bind(ei_ev_mouse_move, NULL, "all", click_moveout, (void *) widget);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
	ei_app_invalidate_rect(&(widget->screen_location));
        return EI_TRUE;
}

ei_bool_t click_moveout(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_widget_t *ancien_widget = (ei_widget_t *) user_param;
        if (widget != ancien_widget) {
                change_relief(ancien_widget);
                ei_unbind(ei_ev_mouse_move, NULL, "all", click_moveout, user_param);
                ei_unbind(ei_ev_mouse_buttonup, ancien_widget, NULL, unclick_button, NULL);
                ei_bind(ei_ev_mouse_move, ancien_widget, NULL, click_movein, NULL);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", unclick, user_param);
		ei_app_invalidate_rect(&(widget->screen_location));
                return EI_TRUE;
        }
        return EI_FALSE;
}

ei_bool_t click_movein(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        change_relief(widget);
        ei_unbind(ei_ev_mouse_move, widget, NULL, click_movein, NULL);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick, (void *) widget);
        ei_bind(ei_ev_mouse_move, NULL, "all", click_moveout, (void *) widget);
        ei_bind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        ei_app_invalidate_rect(&(widget->screen_location));
        return EI_TRUE;
}

ei_bool_t unclick(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, (ei_widget_t *) user_param, NULL, click_movein, NULL);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick, user_param);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        return EI_FALSE;
}

ei_bool_t unclick_button(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "all", click_moveout, widget);
        ei_unbind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
	ei_app_invalidate_rect(&(widget->screen_location));
        change_relief(widget);
        ei_button_t *button = (ei_button_t *) widget;
        if (button->callback != NULL) {
                (*button->callback)(widget, event, button->user_param);
        }
        return EI_TRUE;
}


// Fonction traitante interne pour les top_level

/*** Fermeture ***/

ei_bool_t close_toplevel(ei_widget_t *widget, ei_event_t *event, void *user_param)
{
        ei_toplevel_t *toplevel = (ei_toplevel_t *) user_param;
	ei_app_invalidate_rect(&(widget->screen_location));
        ei_widget_destroy((ei_widget_t *) toplevel);
        return EI_TRUE;
}

/*** Déplacement ***/

ei_bool_t click_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        /* Gestion du déplacement */
        if (event->param.mouse.where.y < widget->screen_location.top_left.y + ((ei_toplevel_t *) widget)->height_header) {
                last_pos = event->param.mouse.where;
                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", move_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", unclick_toplevel, (void *) widget);
        }

        /* Gestion du redimmensionnement */
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        ei_point_t resize_corner = ei_point(widget->screen_location.size.width,
                                            widget->screen_location.size.height);
        resize_corner = ei_point_add(resize_corner,
                                     widget->screen_location.top_left);
        int resize_zone_height;
        if (toplevel->border_width > 2){
                resize_zone_height = 2*toplevel->border_width;
        } else {
                resize_zone_height = 4;
        }
        resize_corner.x -= resize_zone_height;
        resize_corner.y -= resize_zone_height;

        if ((event->param.mouse.where.y > resize_corner.y)
            && (event->param.mouse.where.x > resize_corner.x)){
                last_pos = event->param.mouse.where;
                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", resize_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", unclick_toplevel, (void *) widget);
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
        ei_unbind(ei_ev_mouse_move, NULL, "all", move_toplevel, user_param);
        ei_unbind(ei_ev_mouse_move, NULL, "all", resize_toplevel, user_param);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick_toplevel, user_param);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        return EI_FALSE;
}

/*** Redimensionnement ***/
ei_bool_t resize_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param){
        ei_widget_t *toplevel = (ei_widget_t *) user_param;
        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        int new_size_width = ((ei_placer_param_t *) toplevel->geom_params)->width + diff_x;
        int new_size_height = ((ei_placer_param_t *) toplevel->geom_params)->height + diff_y;

        ei_point_t resize_corner = ei_point(widget->screen_location.size.width,
                                            widget->screen_location.size.height);
        resize_corner = ei_point_add(resize_corner,
                                     widget->screen_location.top_left);
        int resize_zone_height;
        if (((ei_toplevel_t *) toplevel)->border_width > 2){
                resize_zone_height = 2*(((ei_toplevel_t *) toplevel)->border_width);
        } else {
                resize_zone_height = 4;
        }
        resize_corner.x -= resize_zone_height;
        resize_corner.y -= resize_zone_height;

        /* -- Width -- */
        /* On vérifie que la fenêtre est reimmensionnable (axes et taille min)*/
        if (((ei_toplevel_t *) toplevel)->min_size->width <= new_size_width
            && (((ei_toplevel_t *) toplevel)->resizable == ei_axis_x
                || ((ei_toplevel_t *) toplevel)->resizable == ei_axis_both)
        /* Il faut aussi que si la taille min est atteinte, alors on ne recommence
         le redimmensionnement que si l'on est assez près du bord considére*/
            && (((ei_placer_param_t *) toplevel->geom_params)->width  > ((ei_toplevel_t *) toplevel)->min_size->width
                || (((ei_placer_param_t *) toplevel->geom_params)->width <= ((ei_toplevel_t *) toplevel)->min_size->width
                    && event->param.mouse.where.x >= resize_corner.x))){

                ((ei_placer_param_t *) toplevel->geom_params)->width = new_size_width;
        }
        
        /* -- Height -- */
        if (((ei_toplevel_t *) toplevel)->min_size->height <= new_size_height
            && (((ei_toplevel_t *) toplevel)->resizable == ei_axis_y
                || ((ei_toplevel_t *) toplevel)->resizable == ei_axis_both)
            && (((ei_placer_param_t *) toplevel->geom_params)->height  > ((ei_toplevel_t *) toplevel)->min_size->height
                || (((ei_placer_param_t *) toplevel->geom_params)->height <= ((ei_toplevel_t *) toplevel)->min_size->height
                    && event->param.mouse.where.y >= resize_corner.y))){

                ((ei_placer_param_t *) toplevel->geom_params)->height = new_size_height;
        }
        last_pos = event->param.mouse.where;
        return EI_TRUE;
}
