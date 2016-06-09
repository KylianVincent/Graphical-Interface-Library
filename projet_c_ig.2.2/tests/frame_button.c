#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"


/*
 * ei_main --
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
	ei_size_t	screen_size		= {1500, 900};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};
	ei_color_t      button_color		= {0x00, 0xAA, 0x00, 0xff};
	ei_widget_t*	frame;
	ei_size_t	frame_size		= {1500,35};
	int		frame_x			= 150;
	int		frame_y			= 200;
	ei_color_t	frame_color		= {0x00, 0x00, 0xAA, 0xff};
	ei_relief_t	frame_relief		= ei_relief_raised;
	int		frame_border_width	= 1;

	/*arrière plan bureau*/
	ei_app_create(&screen_size, EI_FALSE);
        ei_surface_t img = hw_image_load("wallpaper.jpg", ei_app_root_surface());
        ei_anchor_t anch_img = ei_anc_northwest;
        ei_rect_t *rect_img = calloc(1, sizeof(ei_rect_t));
        ei_anchor_t anch2 = ei_anc_south;
        rect_img->top_left.x=0;rect_img->top_left.y=0; 
        rect_img->size.width=1920; rect_img->size.height=1080;
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, &anch2, &img, &rect_img, &anch_img); 
        
	/* barre des taches */
	frame = ei_widget_create("frame", ei_app_root_widget());
	ei_frame_configure(frame, &frame_size, &frame_color,
			    &frame_border_width, &frame_relief, NULL, NULL, NULL, NULL,
			    NULL, NULL, NULL);
        int x=0;
        int y = 863;
        float relx =0;
        float rely =0;
        ei_anchor_t anc_frame = ei_anc_southeast;
	ei_place(frame, NULL, &x, &y, NULL, NULL, &relx, &rely, NULL, NULL );

        ei_widget_t* button;
        button = ei_widget_create("button", frame);
        char* text = "Demarrer";
        ei_anchor_t anch = ei_anc_center;
        ei_button_configure(button, NULL, &button_color, NULL, NULL, NULL, &text, NULL, NULL, &anch, NULL, NULL, NULL, NULL, NULL);
        ei_place(button, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
