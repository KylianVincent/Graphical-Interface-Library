#include "ei_eventmanager.h"
#include "ei_types.h"
#include "ei_application.h"
#include "ei_classes.h"
#include "ei_geometryclasses.h"
#include "ei_application.h"
#include "ei_widget.h"
#include "ei_globs.h"
#include <math.h>

/*optimisation affichage*/
ei_rect_t update;
ei_widget_t *root;

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
	/*optimisation affichage*/
	root=ei_app_root_widget();
	update=intersect_clipper(widget->screen_location,*(root->content_rect)); 
	ei_app_invalidate_rect(&update);
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
		/*optimisation affichage*/
		root=ei_app_root_widget();
		update=intersect_clipper(ancien_widget->screen_location,*(root->content_rect));
		ei_app_invalidate_rect(&update);
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
	/*optimisation affichage*/
	root=ei_app_root_widget();
	update=intersect_clipper(widget->screen_location,*(root->content_rect));
	ei_app_invalidate_rect(&update);
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
        change_relief(widget);
        ei_button_t *button = (ei_button_t *) widget;
        if (button->callback != NULL) {
                (*button->callback)(widget, event, button->user_param);
        }
	/*optimisation de l'affichage*/
	root=ei_app_root_widget();
	update=intersect_clipper(widget->screen_location,*(root->content_rect));
	ei_app_invalidate_rect(&update);

        return EI_TRUE;
}


// Fonction traitante interne pour les top_level

/*** Fermeture ***/

ei_bool_t close_toplevel(ei_widget_t *widget, ei_event_t *event, void *user_param)
{       /*optimisation affichage*/
        ei_widget_t *toplevel = (ei_widget_t *) user_param;
	update.size.width = toplevel->screen_location.size.width+1;
	update.size.height = toplevel->screen_location.size.height+1;
	update.top_left.x = toplevel->screen_location.top_left.x;
	update.top_left.y = toplevel->screen_location.top_left.y;
	root = ei_app_root_widget();
	update = intersect_clipper(update,*(root->content_rect));
	ei_app_invalidate_rect(&update);
        ei_widget_destroy(toplevel);
        return EI_TRUE;
}

/*** Changement focus ***/

ei_bool_t change_focus(ei_widget_t *widget)
{
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        ei_widget_t *tail = widget;
        if (toplevel->closable) {
                tail = tail->next_sibling;
        }
        if (toplevel->resizable) {
                tail = tail->next_sibling;
        }
        if (widget->parent == NULL || widget->parent->children_tail == tail) {
                return EI_FALSE;
        }
        ei_widget_t *prec = widget->parent->children_head;
        while (prec != widget && prec->next_sibling != widget) {
                prec = prec->next_sibling;
        }
        if (prec == widget) {
                widget->parent->children_head = tail->next_sibling;
        }
        else {
                prec->next_sibling = tail->next_sibling;
        }
        widget->parent->children_tail->next_sibling = widget;
        tail->next_sibling = NULL;
        widget->parent->children_tail = tail;
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

        if ((event->param.mouse.where.y >= resize_corner.y)
            && (event->param.mouse.where.x >= resize_corner.x)){
                last_pos = event->param.mouse.where;
                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", resize_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", unclick_toplevel, (void *) widget);
        }
        /* Si on change le focus de la fenetre, il faut mettre à jour */
        return change_focus(widget);
}

ei_bool_t move_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_widget_t *toplevel = (ei_widget_t *) user_param;

        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        /* ((ei_placer_param_t *) toplevel->geom_params)->x += diff_x; */
        /* ((ei_placer_param_t *) toplevel->geom_params)->y += diff_y; */
        int new_pos_x = toplevel->screen_location.top_left.x + diff_x;
        int new_pos_y = toplevel->screen_location.top_left.y + diff_y;
        ei_place(toplevel, NULL, &new_pos_x, &new_pos_y, NULL, NULL, NULL, NULL, NULL, NULL);
        last_pos = event->param.mouse.where;
	/*optimisation de l'affichage*/
	if(toplevel->screen_location.top_left.x + (diff_x-abs(diff_x))/2<0){
		update.top_left.x=0;
	}else{
		update.top_left.x=toplevel->screen_location.top_left.x + (diff_x-abs(diff_x))/2;
	}

	if(toplevel->screen_location.top_left.y + (diff_y-abs(diff_y))/2<0){
		update.top_left.y=0;
	}else{
		update.top_left.y=toplevel->screen_location.top_left.y + (diff_y-abs(diff_y))/2;
	}

	if(1+toplevel->screen_location.size.width + abs(diff_x)+update.top_left.x >taille_root_frame.width){
		update.size.width=taille_root_frame.width-update.top_left.x;
	}else{
		update.size.width= toplevel->screen_location.size.width + abs(diff_x)+1;
	}

	if(1+toplevel->screen_location.size.height + abs(diff_y)+update.top_left.y >taille_root_frame.height){
		update.size.height=taille_root_frame.height-update.top_left.y;
	}else{
		update.size.height= toplevel->screen_location.size.height + abs(diff_y)+1;
	}
	ei_app_invalidate_rect(&update);

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
        ei_widget_t *widget_bis = (ei_widget_t *) user_param;
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget_bis;
        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        int new_size_width = widget_bis->content_rect->size.width + diff_x;
        int new_size_height = widget_bis->content_rect->size.height + diff_y;

        /* Calcul du point nord-est de la zone de redimmensionnement */
        ei_point_t resize_corner_min = widget_bis->content_rect->top_left;
        int bords = (toplevel->border_width>2)?toplevel->border_width:2;
        resize_corner_min.x += toplevel->min_size->width - bords;
        resize_corner_min.y += toplevel->min_size->height - bords;

        

        /* -- Width -- */
        /* On vérifie que la fenêtre est redimmensionnable (axes et taille min)*/
        if (toplevel->resizable == ei_axis_x
                     || toplevel->resizable == ei_axis_both)
        {
                if (event->param.mouse.where.x < resize_corner_min.x) {
                        new_size_width = toplevel->min_size->width;
                        last_pos.x = resize_corner_min.x;
                }
                else if (new_size_width >= toplevel->min_size->width) {
                        last_pos.x = event->param.mouse.where.x;
                }
        }
        /* -- Height -- */
        if (toplevel->resizable == ei_axis_y
                || toplevel->resizable == ei_axis_both)
        {
                if (event->param.mouse.where.y < resize_corner_min.y) {
                        new_size_height = toplevel->min_size->height;
                        last_pos.y = resize_corner_min.y;
                }
                else if (new_size_height >= toplevel->min_size->height) {
                        last_pos.y = event->param.mouse.where.y;
                }
        }

        ei_place(widget_bis, NULL, NULL, NULL, &new_size_width, &new_size_height, NULL, NULL, NULL, NULL);

	/*optimisation de l'affichage*/
	update.size.width = widget_bis->screen_location.size.width + 1 + (diff_x+abs(diff_x))/2;
        update.size.height = widget_bis->screen_location.size.height + 1 + (diff_y+abs(diff_y))/2;
	update.top_left = widget_bis->screen_location.top_left;
	root=ei_app_root_widget();
	update=intersect_clipper(update,*(root->content_rect));
	ei_app_invalidate_rect(&update);
        return EI_TRUE;
}
