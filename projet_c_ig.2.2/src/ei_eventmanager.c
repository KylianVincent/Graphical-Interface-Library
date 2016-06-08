#include "ei_eventmanager.h"
#include "ei_types.h"
#include "ei_application.h"
#include "ei_classes.h"
#include "ei_geometryclasses.h"
#include "ei_application.h"
#include "ei_widget.h"
#include "ei_globs.h"
#include "ei_entry.h"
#include <math.h>
#include <string.h>

ei_linked_bind_t **binds_event;
/* Utile pour le déplacement et redimmensionnement de fenetre */
ei_point_t last_pos;

void init_binds_event()
{
        binds_event = calloc(8, sizeof(ei_linked_bind_t *));
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "entry", focus_entry, NULL);
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

//fonction de liberation de binds event
void free_binds_event()
{
	ei_linked_bind_t *cour;
	ei_linked_bind_t *tmp;
	for (int8_t i=0; i<8; i++){
		cour=binds_event[i];
		while (cour != NULL){
			tmp=cour;
			cour=cour->next;
			free(tmp);
		}
	}
}


// Fonction qui traite un évènement
ei_bool_t handle_event(ei_event_t* event)
{
        ei_widget_t *widget = get_widget_of_pixel(event->param.mouse.where);
        ei_linked_bind_t *cour = binds_event[event->type];
        ei_bool_t stop = EI_FALSE;
        while (!stop && cour != NULL) {
                ei_linked_bind_t *next = cour->next;
                /** Evenements externes **/
                if (cour->widget == widget){
                        stop = (*cour->callback)(widget, event, 
						 cour->user_param);
                
			/** Evenements interne **/
		}else if (cour->tag != NULL && !strcmp(cour->tag,
						       widget->wclass->name)){
                        stop = (*cour->callback)(widget, event, 
						 cour->user_param);

                }else if (cour->tag != NULL && !strcmp(cour->tag,"all")){
                        stop = (*cour->callback)(widget, event,cour->user_param);
                }
                cour = next;
        }
        return stop;
}

// Fonction qui quitte lorqu'on appuye sur échap
ei_bool_t escape(ei_widget_t *widget, ei_event_t* event, void *user_param)
{
        if (event->type == ei_ev_keydown 
	    && event->param.key.key_sym == SDLK_ESCAPE)
        {
                ei_app_quit_request();
                return EI_TRUE;
        }
        return EI_FALSE;
}

// Fonction traitante interne pour les boutons

void change_relief(ei_widget_t *widget) 
{
        int rel = ((ei_button_t*) widget)->relief;
        if (rel == 1){
                ((ei_button_t*) widget)->relief++;
        }else if (rel == 2){
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
	ei_app_invalidate_rect(&(widget->screen_location));
        return EI_TRUE;
}

ei_bool_t click_moveout(ei_widget_t* widget, ei_event_t* event, 
			void * user_param)
{
        ei_widget_t *ancien_widget = (ei_widget_t *) user_param;
        if (widget != ancien_widget) {
                change_relief(ancien_widget);

                ei_unbind(ei_ev_mouse_move, NULL, "all", 
			  click_moveout, user_param);
                ei_unbind(ei_ev_mouse_buttonup, ancien_widget, 
			  NULL, unclick_button, NULL);
                ei_bind(ei_ev_mouse_move, ancien_widget, NULL, 
			click_movein, NULL);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", 
			unclick, user_param);

		/*optimisation affichage*/
		ei_app_invalidate_rect(&(ancien_widget->screen_location));
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
	ei_app_invalidate_rect(&(widget->screen_location));
        return EI_TRUE;
}

ei_bool_t unclick(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_move, (ei_widget_t *) user_param, 
		  NULL, click_movein, NULL);

        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick, user_param);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        return EI_TRUE;
}

ei_bool_t unclick_button(ei_widget_t* widget, ei_event_t* event, 
			 void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "all", click_moveout, widget);
        ei_unbind(ei_ev_mouse_buttonup, widget, NULL, unclick_button, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "button", click_button, NULL);
        change_relief(widget);
	/*optimisation de l'affichage*/
	ei_app_invalidate_rect(&(widget->screen_location));

        ei_button_t *button = (ei_button_t *) widget;
        if (button->callback != NULL){
                (*button->callback)(widget, event, button->user_param);
        }

        return EI_TRUE;
}


/*Fonction traitante interne pour les top_level*/

/*Fermeture*/

ei_bool_t close_toplevel(ei_widget_t *widget, ei_event_t *event, 
			 void *user_param)
{       
        /*optimisation affichage*/
        ei_widget_t *toplevel = (ei_widget_t *) user_param;
	ei_app_invalidate_rect(&(toplevel->screen_location));
        ei_widget_destroy(toplevel);
        return EI_TRUE;
}

/*Changement de Focus */

void change_focus(ei_widget_t *widget)
{
        ei_widget_t *tail = widget;
        tail = tail->next_sibling->next_sibling;
        if (widget->parent == NULL || widget->parent->children_tail == widget){
                return;
        }

        ei_widget_t *prec = widget->parent->children_head;
        while (prec != widget && prec->next_sibling != widget) {
                prec = prec->next_sibling;
        }
        if (prec == widget){
                widget->parent->children_head = tail->next_sibling;
        }else{
                prec->next_sibling = tail->next_sibling;
        }
        ei_widget_t *cour = widget->parent->children_tail;
        while (cour->next_sibling != NULL)
                cour = cour->next_sibling;
        cour->next_sibling = widget;
        tail->next_sibling = NULL;
        widget->parent->children_tail = widget;
        /* On ajoute le rectangle à mettre à jour */
	ei_app_invalidate_rect(&(widget->screen_location));
        return;
}

/*Déplacement*/

ei_bool_t click_toplevel(ei_widget_t* widget, ei_event_t* event, 
			 void * user_param)
{
        /* Si on change le focus de la fenetre, il faut mettre à jour */
        change_focus(widget);
        /* Gestion du déplacement */
        if (event->param.mouse.where.y < widget->screen_location.top_left.y 
	    + ((ei_toplevel_t *) widget)->height_header){

                last_pos = event->param.mouse.where;

                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", 
			  click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", 
			move_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", 
			unclick_toplevel, (void *) widget);

                return EI_TRUE;
        }

        /* Gestion du redimensionnement */
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        if (!toplevel->resizable) {
                return EI_FALSE;
        }
        ei_point_t resize_corner = ei_point(widget->screen_location.size.width,
                                            widget->screen_location.size.height);
        resize_corner = ei_point_add(resize_corner,
				     widget->screen_location.top_left);
        int resize_zone_height;
        if (toplevel->border_width >= min_resize_zone){
                resize_zone_height = 2*toplevel->border_width;
        }else{
                resize_zone_height = 2*min_resize_zone;
        }
        resize_corner.x -= resize_zone_height;
        resize_corner.y -= resize_zone_height;

        if ((event->param.mouse.where.y >= resize_corner.y)
            && (event->param.mouse.where.x >= resize_corner.x)){
                last_pos = event->param.mouse.where;

                ei_unbind(ei_ev_mouse_buttondown, NULL, "toplevel", 
			  click_toplevel, NULL);
                ei_bind(ei_ev_mouse_move, NULL, "all", 
			resize_toplevel, (void *) widget);
                ei_bind(ei_ev_mouse_buttonup, NULL, "all", 
			unclick_toplevel, (void *) widget);

                return EI_TRUE;
        }
        return EI_FALSE;
}

ei_bool_t move_toplevel(ei_widget_t* widget, ei_event_t* event, 
			void * user_param)
{
        ei_widget_t *toplevel = (ei_widget_t *) user_param;

        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        int new_pos_x = toplevel->screen_location.top_left.x + diff_x;
        int new_pos_y = toplevel->screen_location.top_left.y + diff_y;
        
        if (widget->parent != NULL) {
                new_pos_x -= widget->parent->content_rect->top_left.x;
                new_pos_y -= widget->parent->content_rect->top_left.y;
        }
        /* On doit mettre à zéro la valeur de la position relative */
        float zero = 0.0;
        ei_place(toplevel, NULL, &new_pos_x, &new_pos_y, NULL, NULL, 
		 &zero, &zero, NULL, NULL);

        last_pos = event->param.mouse.where;
        return EI_TRUE;
}


ei_bool_t unclick_toplevel(ei_widget_t* widget, ei_event_t* event, 
			   void * user_param)
{
        ei_unbind(ei_ev_mouse_move, NULL, "all", move_toplevel, user_param);
        ei_unbind(ei_ev_mouse_move, NULL, "all", resize_toplevel, user_param);
        ei_unbind(ei_ev_mouse_buttonup, NULL, "all", unclick_toplevel, 
		  user_param);
        ei_bind(ei_ev_mouse_buttondown, NULL, "toplevel", click_toplevel, NULL);
        return EI_TRUE;
}

/*Redimensionement*/
ei_bool_t resize_toplevel(ei_widget_t* widget, ei_event_t* event, 
			  void * user_param){
        ei_widget_t *widget_bis = (ei_widget_t *) user_param;
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget_bis;
        int diff_x = event->param.mouse.where.x - last_pos.x;
        int diff_y = event->param.mouse.where.y - last_pos.y;
        int new_size_width = widget_bis->content_rect->size.width;
        int new_size_height = widget_bis->content_rect->size.height;

        /* Calcul du point nord-est de la zone de redimensionement */
        ei_point_t resize_corner_min = widget_bis->content_rect->top_left;
        int bords = toplevel->border_width;
        int decalage = (bords >= min_resize_zone)?2*bords:2*min_resize_zone;
        resize_corner_min.x += toplevel->min_size->width + bords - decalage;
        resize_corner_min.y += toplevel->min_size->height + bords - decalage;

        /* -- Width -- */
        /* On vérifie que la fenêtre est redimmensionnable (axes et taille min)*/
        if (toplevel->resizable == ei_axis_x
                     || toplevel->resizable == ei_axis_both)
        {
                new_size_width += diff_x;
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
                new_size_height += diff_y;
                if (event->param.mouse.where.y < resize_corner_min.y) {
                        new_size_height = toplevel->min_size->height;
                        last_pos.y = resize_corner_min.y;
                }
                else if (new_size_height >= toplevel->min_size->height) {
                        last_pos.y = event->param.mouse.where.y;
                }
        }
        /* On doit mettre à zéro la valeur de la taille relative */
        float zero = 0.0;
        ei_place(widget_bis, NULL, NULL, NULL, &new_size_width, 
		 &new_size_height, NULL, NULL, &zero, &zero);
        return EI_TRUE;
}

/*----------- ENTRY ---------------*/

ei_bool_t focus_entry(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        ei_unbind(ei_ev_mouse_buttondown, NULL, "entry", focus_entry, NULL);
        focus = widget;
        ei_bind(ei_ev_keydown, NULL, "all", treat_key_down, NULL);
        ei_bind(ei_ev_keyup, NULL, "all", treat_key_up, NULL);
        ei_bind(ei_ev_mouse_buttondown, NULL, "all", unfocus_entry, NULL);
        return EI_TRUE;
}

ei_bool_t unfocus_entry(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        if (strcmp(widget->wclass->name,"entry")) {
                ei_unbind(ei_ev_mouse_buttondown, NULL, "all", unfocus_entry, NULL);
                ei_unbind(ei_ev_keydown, NULL, "all", treat_key_down, NULL);
                ei_unbind(ei_ev_keyup, NULL, "all", treat_key_up, NULL);
                ei_bind(ei_ev_mouse_buttondown, NULL, "entry", focus_entry, NULL);
                focus = NULL;
                return EI_TRUE;
        }
        else if (widget != focus) {
                focus = widget;
                return EI_TRUE;
        }
        return EI_FALSE;
}

ei_bool_t treat_key_down(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        if (focus != NULL) {
                ei_entry_t *entry = (ei_entry_t *) focus;
                if ((event->param.key.key_sym >= SDLK_EXCLAIM && 
                     event->param.key.key_sym <= SDLK_z) ||
                        event->param.key.key_sym == SDLK_SPACE)
                {
                        /* Le texte ne doit pas dépasser la taille maximale
                           définie (0 signifie une taille non limitée */
                        if (entry->text == NULL ||
                            entry->char_max == 0 ||
                            strlen(entry->text) < entry->char_max)
                        {
                                int majuscule = 0;
                                if (event->param.key.key_sym != SDLK_SPACE &&
                                    event->param.key.key_sym >= SDLK_a &&
                                    capital)
                                {
                                        majuscule = 'A'-'a';
                                }
                                if (entry->text != NULL) {
                                        int len = strlen(entry->text);
                                        len++;
                                        char *temp = entry->text;
                                        entry->text = calloc(len+1, sizeof(char));
                                        strcpy(entry->text, temp);
                                        free(temp);
                                        temp = entry->text;
                                        temp[len-1] = event->param.key.key_sym+majuscule;
                                }
                                else {
                                        entry->text = calloc(2, sizeof(char));
                                        char *temp = entry->text;
                                        temp[0] = event->param.key.key_sym+majuscule;
                                }
                        }
                        return EI_TRUE;
                }
                else if (event->param.key.key_sym == SDLK_BACKSPACE &&
                    entry->text != NULL) {
                        int len = strlen(entry->text);
                        if (len > 1) {
                                len--;
                                char *txt = entry->text;
                                txt[len] = '\0';
                        }
                        else {
                                free(entry->text);
                                entry->text = NULL;
                        }
                        return EI_TRUE;
                }
                else if (event->param.key.key_sym == SDLK_LSHIFT ||
                         event->param.key.key_sym == SDLK_RSHIFT)
                {
                        capital = (capital)?EI_FALSE:EI_TRUE;
                        return EI_TRUE;
                }
        }
        return EI_FALSE;
}

ei_bool_t treat_key_up(ei_widget_t* widget, ei_event_t* event, void * user_param)
{
        if (focus != NULL) {
                if (event->param.key.key_sym == SDLK_LSHIFT ||
                    event->param.key.key_sym == SDLK_RSHIFT)
                {
                        capital = (capital)?EI_FALSE:EI_TRUE;
                        return EI_TRUE;
                }
        }
        return EI_FALSE;
}
