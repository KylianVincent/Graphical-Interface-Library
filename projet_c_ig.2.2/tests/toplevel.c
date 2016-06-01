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
	ei_size_t	screen_size		= {600, 600};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

	ei_widget_t*	frame;
	ei_size_t	frame_size		= {300,200};
	int		frame_x			= 150;
	int		frame_y			= 200;
	ei_color_t	frame_color		= {0x88, 0x88, 0x88, 0xff};
	ei_relief_t	frame_relief		= ei_relief_raised;
	int		frame_border_width	= 6;

        /* top_level configuration */
        ei_widget_t*	toplevel;
        ei_size_t       toplevel_size           = {350, 250};
        ei_color_t      toplevel_color          = {0x99, 0x99, 0x99, 0xff};
	int		toplevel_x		= 125;
	int		toplevel_y		= 150;

	/* Create the application and change the color of the background. */
	ei_app_create(&screen_size, EI_FALSE);
        ei_surface_t img = hw_image_load("test.jpg", ei_app_root_surface());
        ei_anchor_t anch_img = ei_anc_northwest;
        ei_rect_t *rect_img = calloc(1, sizeof(ei_rect_t));
        char* text2 = "En bas";
        ei_anchor_t anch2 = ei_anc_south;
        rect_img->top_left.x=470;rect_img->top_left.y=230; 
        rect_img->size.width=70; rect_img->size.height=70;
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, &text2, NULL, NULL, &anch2, NULL, NULL, NULL);
        
	/* Create, configure and place the toplevel on screen. */
        toplevel = ei_widget_create("toplevel", ei_app_root_widget());
        ei_toplevel_configure(toplevel, &toplevel_size, &toplevel_color, NULL, NULL, NULL, NULL, NULL);
        ei_place(toplevel, NULL, &toplevel_x, &toplevel_y, NULL, NULL, NULL, NULL, NULL, NULL);

	/* Create, configure and place the frame on screen. */
	frame = ei_widget_create("frame", toplevel);
	ei_frame_configure(frame, &frame_size, &frame_color,
			    &frame_border_width, &frame_relief, NULL, NULL, NULL, NULL,
			    &img, &rect_img, &anch_img);
	ei_place(frame, NULL, &frame_x, &frame_y, NULL, NULL, NULL, NULL, NULL, NULL );
        ei_widget_t* button;
        button = ei_widget_create("button", frame);
        char* text = "Cliquez ici";
        ei_anchor_t anch = ei_anc_center;
        ei_button_configure(button, NULL, &root_bgcol, NULL, NULL, NULL, &text, NULL, NULL, &anch, NULL, NULL, NULL, NULL, NULL);
        ei_place(button, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
