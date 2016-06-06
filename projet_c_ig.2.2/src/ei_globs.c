#include "ei_globs.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_application.h"
#include "ei_utils.h"
#include "ei_globs.h"
#include "ei_eventmanager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2){
	ei_rect_t result;
	ei_rect_t tmp;
	if (rect1.top_left.x > rect2.top_left.x) {
                tmp= rect1;
                rect1 = rect2;
                rect2 = tmp;
	}

	result.top_left.y=(rect1.size.height <= rect2.size.height)?rect1.size.height:rect2.size.height;
	result.top_left.x=rect1.top_left.x ;

	result.size.width=(rect1.size.width + rect1.top_left.x <= rect2.size.width + rect2.top_left.x)?
		rect2.size.width + rect2.top_left.x:rect1.size.width + rect1.top_left.x;
	result.size.height=(rect1.size.height + rect1.top_left.y <= rect2.size.height + rect2.top_left.y)?
		rect2.size.height + rect2.top_left.y:rect1.size.height + rect1.top_left.y;


	return result;
}
