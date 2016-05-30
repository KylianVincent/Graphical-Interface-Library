#include "ei_application.h"
#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_geometryclasses.h"
#include <stdlib.h>
#include <stdio.h>

ei_surface_t main_window;
ei_surface_t main_window_picking;
ei_widget_t* root_frame=NULL;


/* void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen); */
void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen)
{
        if (!fullscreen && main_window_size == NULL){
                perror("La taille de la fenêtre n'est pas définie.\n");
                exit(1);
        }

        hw_init();
	main_window = hw_create_window(main_window_size, fullscreen);
	main_window_picking=hw_surface_create(main_window,main_window_size,EI_TRUE);
        /* Creation du geometry manager "placer" */
        ei_register_placer_manager();
        
        /* Creation du widget frame */
        ei_frame_register_class();

        /* Initialisation de root_frame */
        root_frame = ei_widget_create("frame", NULL);
        ei_frame_configure(root_frame, main_window_size, &ei_default_background_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        ei_place(root_frame, NULL, NULL, NULL, &(main_window_size->width), &(main_window_size->height), NULL, NULL, NULL, NULL);
        
}

/* void ei_app_free(); */
void ei_app_free(){
        ei_widget_destroy(root_frame);
        hw_surface_free(main_window_picking);
        hw_surface_free(main_window);
        hw_quit();
}

void ei_app_run_rec(ei_widget_t* w)
{
        if (w == NULL) {
                return;
        }
        
        (*w->geom_params->manager->runfunc)(w);
        if (w->parent != NULL) {
                (*w->wclass->drawfunc)(w, main_window, main_window_picking, w->parent->content_rect);
        }
        else {
                (*w->wclass->drawfunc)(w, main_window, main_window_picking, w->content_rect);
        }
        ei_widget_t* cour = w->children_head;
        while (cour != NULL) {
                ei_app_run_rec(cour);
                cour = cour->next_sibling;
        }
}

/* void ei_app_run(); */
void ei_app_run(){
        ei_app_run_rec(root_frame);
        getchar();
}

/* void ei_app_invalidate_rect(ei_rect_t* rect); */


/* void ei_app_quit_request(); */


/* ei_widget_t* ei_app_root_widget(); */

ei_widget_t* ei_app_root_widget()
{
        return((ei_widget_t *) root_frame);
}

/* ei_surface_t ei_app_root_surface(); */
