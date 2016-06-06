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

ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2){
	ei_rect_t result;
	ei_rect_t tmp;
	if (rect1.top_left.x > rect2.top_left.x) {
                tmp= rect1;
                rect1 = rect2;
                rect2 = tmp;
	}
	
	int diffx=rect1.top_left.x-rect2.top_left.x;
	int diffy=rect1.top_left.y-rect2.top_left.y;

	if (diffy <=0){
		result.top_left=rect1.top_left;
		result.size.width=rect1.size.width + rect2.size.width - diffx;
		result.size.height=rect1.size.height + rect2.size.height - diffy;
	}else{
		result.top_left.x=rect1.top_left.x;
		result.top_left.y=rect1.top_left.y + diffy + rect2.top_left.y;
		result.size.width=rect1.size.width + rect2.size.width - diffx;
		result.size.height=rect1.size.height + rect2.size.height + diffy;
	}
	return result;
}
