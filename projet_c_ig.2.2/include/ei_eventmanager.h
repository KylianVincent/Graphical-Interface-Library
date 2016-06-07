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
//liberation de la structure binds_event
void free_binds_event();
// Fonction calculant renvoyant le widget correspondant à un pixel
ei_widget_t *get_widget_of_pixel(ei_point_t where);

// Fonction qui traite un évènement
ei_bool_t handle_event(ei_event_t* event);

// Fonction qui quitte lorqu'on appuye sur échappe
ei_bool_t escape(ei_widget_t *widget, ei_event_t* event, void *user_param);

// Fonction traitante interne pour les boutons
ei_bool_t click_button(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t click_moveout(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t click_movein(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t unclick(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t unclick_button(ei_widget_t* widget, ei_event_t* event, void * user_param);

// Fonction traitante interne pour les top_level

ei_bool_t close_toplevel(ei_widget_t *widget, ei_event_t *event, void *user_param);

ei_bool_t click_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t move_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t unclick_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param);

ei_bool_t resize_toplevel(ei_widget_t* widget, ei_event_t* event, void * user_param);
