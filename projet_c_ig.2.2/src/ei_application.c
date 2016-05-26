#include "ei_application.h"
#include "hw_interface.h"
#include <stdlib.h>
#include <stdio.h>


/* void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen); */
void ei_app_create(ei_size_t* main_window_size, ei_bool_t fullscreen){

	ei_surface_t main_window;
        hw_init();
	main_window = hw_create_window(main_window_size, fullscreen);
        
}

/* void ei_app_free(); */
void ei_app_free(){
        /* Corps vide */
}

/* void ei_app_run(); */
void ei_app_run(){
        getchar();
}

/* void ei_app_invalidate_rect(ei_rect_t* rect); */


/* void ei_app_quit_request(); */


/* ei_widget_t* ei_app_root_widget(); */

ei_widget_t* ei_app_root_widget()
{
        return(NULL);
}

/* ei_surface_t ei_app_root_surface(); */
