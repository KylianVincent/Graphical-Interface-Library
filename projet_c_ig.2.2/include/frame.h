#include <stdlib.h>
#include <stdio.h>

struct ei_widgetclass_t frame_class;
frame_class.name;
frame_class.allocfunc = frame_allocfunc;
frame_class.releasefunc = frame_releasefunc;
frame_class.drawfunc = frame_drawfunc;
frame_class.setdefaultsfunc = frame_setdefaultsfunc;


void* frame_allocfunc ():

	void* frame_releasefunc (struct ei_widget_t* frame);


void* frame_drawfunc(struct ei_widget_t*	frame,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper);


void * frame_setdefaultsfunc(struct ei_widget_t*       frame);
