#include "ei_application.h"
#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_geometryclasses.h"
#include "ei_classes.h"
#include "ei_event.h"
#include "ei_eventmanager.h"
#include "ei_globs.h"
#include "ei_entry.h"
#include <stdlib.h>
#include <stdio.h>

ei_widget_t* root_frame=NULL;
ei_bool_t run = EI_TRUE;
ei_linked_rect_t *update_rects=NULL;

/* void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen); */
void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen)
{
	init_picking();
        if (!fullscreen && main_window_size == NULL){
                perror("La taille de la fenêtre n'est pas définie.\n");
                exit(1);
        }

        hw_init();
	main_window = hw_create_window(main_window_size, fullscreen);

	main_window_picking = hw_surface_create(main_window, main_window_size,
						EI_TRUE);
        taille_root_frame = *main_window_size;
        /* Creation du geometry manager "placer" */
        ei_register_placer_manager();
        
        /* Creation du widget frame */
        ei_frame_register_class();
        ei_button_register_class();
        ei_toplevel_register_class();
        ei_entry_register_class();
        /* Initialisation du tableau de binds */
        init_binds_event();

        /* Initialisation de root_frame */
        root_frame = ei_widget_create("frame", NULL);

        ei_frame_configure(root_frame, main_window_size, 
			   &ei_default_background_color, NULL, NULL, NULL, NULL,
			   NULL, NULL, NULL, NULL, NULL);

        ei_place(root_frame, NULL, NULL, NULL, &(main_window_size->width), 
		 &(main_window_size->height), NULL, NULL, NULL, NULL);
        
}

/* void ei_app_free(); */
void ei_app_free(){
        ei_widget_destroy(root_frame);
        hw_surface_free(main_window_picking);
        hw_surface_free(main_window);
	free(tab_pick);
	free_binds_event();
        hw_quit();
}

void treat_updates(ei_linked_rect_t *list_rect)
{
        ei_widget_t *root = ei_app_root_widget();
        ei_rect_t limit = *(root->content_rect);
        while (list_rect != NULL)
        {
                list_rect->rect.size.width++;
                list_rect->rect.size.height++;
                list_rect->rect = intersect_clipper(limit,list_rect->rect);
                list_rect = list_rect->next;
        }
}

void ei_app_run_rec(ei_widget_t* w, ei_rect_t* clipper)
{
        if (w == NULL || clipper->size.height <= 0 || clipper->size.width <= 0) 
        {
                return;
        }

        if (w->geom_params != NULL){        
                /* Dessin du widget */
                (*w->wclass->drawfunc)(w, main_window, main_window_picking,
				       clipper);
        }
        ei_widget_t* cour = w->children_head;
        while (cour != NULL) {
                ei_rect_t new_clip = intersect_clipper(*clipper,
						       *(w->content_rect));
                ei_app_run_rec(cour, &new_clip);
                cour = cour->next_sibling;
        }
}

/* void ei_app_run(); */
void ei_app_run(){
        struct ei_event_t* event = calloc(1, sizeof(ei_event_t));
        ei_app_run_rec(root_frame, root_frame->content_rect);
        hw_surface_update_rects(main_window, NULL);
        hw_surface_update_rects(main_window_picking, NULL);
        while (run) {
                hw_event_wait_next(event);
                if (event->type != ei_ev_none && handle_event(event)) {
                        ei_app_run_rec(root_frame, root_frame->content_rect);
                        treat_updates(update_rects);
                        hw_surface_update_rects(main_window, update_rects);
                        hw_surface_update_rects(main_window_picking, 
						update_rects);
			free_linked_rects(&update_rects);
                }
        }
        free(event);
}

/* void ei_app_invalidate_rect(ei_rect_t* rect);*/

void ei_app_invalidate_rect(ei_rect_t* rect){
	ei_linked_rect_t *new_cell=calloc(1,sizeof(ei_linked_rect_t));
	new_cell->rect=*rect;
	new_cell->next=update_rects;
        update_rects = new_cell;
}


/* void ei_app_quit_request(); */
void ei_app_quit_request()
{
        run = EI_FALSE;
}


/* ei_widget_t* ei_app_root_widget(); */

ei_widget_t* ei_app_root_widget()
{
        return((ei_widget_t *) root_frame);
}

/* ei_surface_t ei_app_root_surface(); */
ei_surface_t ei_app_root_surface() 
{
        return main_window;
}
