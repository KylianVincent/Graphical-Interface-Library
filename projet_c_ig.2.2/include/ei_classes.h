#include <stdlib.h>
#include <stdio.h>
#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "hw_interface.h"
#include "ei_utils.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/*Definition du type ei_frame_t*/

typedef struct ei_frame_t{

	ei_widget_t	        widget;
	ei_color_t      	color;
	int		        border_width;
	ei_relief_t		relief;
	char*	                text;
	ei_font_t		text_font;
	ei_color_t		text_color;
	ei_anchor_t		text_anchor;
	ei_surface_t		img;
	ei_rect_t*		img_rect;
	ei_anchor_t		img_anchor;
}ei_frame_t;

/* Definition du type ei_button_t */

typedef struct ei_button_t{
        ei_widget_t             widget;
        ei_color_t              color;
        int                     border_width;
        ei_relief_t             relief;
        char*                   text;
        ei_font_t               text_font;
        ei_color_t              text_color;
        ei_anchor_t		text_anchor;
	ei_surface_t		img;
	ei_rect_t*		img_rect;
	ei_anchor_t		img_anchor;
        int                     corner_radius;
        ei_callback_t           callback;

        void*                   user_param;
}ei_button_t;

/*Definition du type ei_toplevel_t*/

typedef struct ei_toplevel_t{
	ei_widget_t	        widget;
	ei_color_t      	color;
	int			border_width;
	char*			title;
	ei_bool_t		closable;
	ei_axis_set_t		resizable;
	ei_size_t* 		min_size;
        int 			height_header;
}ei_toplevel_t;


/*si ancrage vaut null ou ei_anc_none, alors cas pas défault (northwest)*/

/* Change la couleur sur chaque octet de val */
/* Eclairci ou assombri en fonction de sign (1 ou -1) */
ei_color_t eclaircir_assombrir(ei_color_t color, uint8_t val, int8_t sign);


/*Definition de fonctions spécifiques à la classe frame*/
void* frame_allocfunc ();

void frame_releasefunc (struct ei_widget_t* widget);

void frame_drawfunc(struct ei_widget_t* frame,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper);

void frame_setdefaultsfunc(struct ei_widget_t* widget);


/* Définition de fonctions spécifiques à la classe button */

void* button_allocfunc ();

void button_releasefunc (struct ei_widget_t* widget);

void button_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper);

void button_setdefaultsfunc(struct ei_widget_t* widget);


/* Définition de fonctions spécifiques à la classe button */

void toplevel_geomnotifyfunc (struct ei_widget_t *widget, ei_rect_t rect);

void* toplevel_allocfunc ();

void toplevel_releasefunc (struct ei_widget_t* widget);

void toplevel_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper);

void toplevel_setdefaultsfunc(struct ei_widget_t* widget);
