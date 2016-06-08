#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ei_widget.h"
#include "ei_event.h"

typedef struct ei_linked_bind_t {
        ei_widget_t* widget;
        ei_tag_t tag;
        ei_callback_t callback;
        void* user_param;
        struct ei_linked_bind_t* next;
} ei_linked_bind_t;

void init_binds_event();

ei_linked_bind_t **get_binds_event();

/*Liberation de la structure binds_event*/
void free_binds_event();

/*Fonction calculant et renvoyant le widget correspondant à un pixel donné*/
ei_widget_t *get_widget_of_pixel(ei_point_t where);

/*Fonction traitant un évènement*/
ei_bool_t handle_event(ei_event_t* event);

/*Fonction quittant le programme lors d'un appui sur échap*/
ei_bool_t escape(ei_widget_t *widget, ei_event_t* event, void *user_param);

/*Fonctions traitantes internes pour les boutons*/
ei_bool_t click_button(ei_widget_t* widget, ei_event_t* event, 
		       void * user_param);

ei_bool_t click_moveout(ei_widget_t* widget, ei_event_t* event, 
			void * user_param);

ei_bool_t click_movein(ei_widget_t* widget, ei_event_t* event, 
		       void * user_param);

ei_bool_t unclick(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t unclick_button(ei_widget_t* widget, ei_event_t* event, 
			 void * user_param);

/*Fonctions traitantes internes pour les top_levels*/

ei_bool_t close_toplevel(ei_widget_t *widget, ei_event_t *event, 
			 void *user_param);

ei_bool_t click_toplevel(ei_widget_t* widget, ei_event_t* event, 
			 void * user_param);

ei_bool_t move_toplevel(ei_widget_t* widget, ei_event_t* event, 
			void * user_param);

ei_bool_t unclick_toplevel(ei_widget_t* widget, ei_event_t* event, 
			   void * user_param);

ei_bool_t resize_toplevel(ei_widget_t* widget, ei_event_t* event, 
			  void * user_param);
