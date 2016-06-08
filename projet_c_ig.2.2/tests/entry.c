#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_entry.h"


/*
 * ei_main --
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
	ei_size_t	screen_size		= {600, 600};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

	ei_widget_t*	entry;
	ei_size_t	entry_size		= {300,200};
	int		entry_x			= 20;
	int		entry_y			= 20;
	ei_color_t	entry_color		= {0x88, 0x88, 0x88, 0xff};
	ei_relief_t	entry_relief		= ei_relief_raised;
	int		entry_border_width	= 6;

/* Create the application and change the color of the background. */
	ei_app_create(&screen_size, EI_FALSE);
	/* Create the application and change the color of the background. */
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        
	/* Create, configure and place the entry on screen. */
        entry = ei_widget_create("entry", ei_app_root_widget());
        ei_entry_configure(entry, &entry_size, NULL, NULL, NULL, NULL, NULL);
        ei_place(entry, NULL, &entry_x, &entry_y, NULL, NULL, NULL, NULL, NULL, NULL);
	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
