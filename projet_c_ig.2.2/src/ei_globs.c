#include "ei_globs.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_geometrymanager.h"
#include "ei_types.h"
#include "ei_classes.h"
#include "ei_application.h"
#include "ei_eventmanager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

ei_rect_t union_rect( ei_rect_t rect1, ei_rect_t rect2){
	ei_rect_t result;

	result.top_left.x = (rect1.top_left.x<=rect2.top_left.x)?
                rect1.top_left.x:rect2.top_left.x;
	result.top_left.y = (rect1.top_left.y<=rect2.top_left.y)?
                rect1.top_left.y:rect2.top_left.y;
        int max_y1 = rect1.top_left.y + rect1.size.height;
        int max_y2 = rect2.top_left.y + rect2.size.height;
        int max_x1 = rect1.top_left.x + rect1.size.width;
        int max_x2 = rect2.top_left.x + rect2.size.width;
        result.size.width = ((max_x1>=max_x2)?max_x1:max_x2)-result.top_left.x+1;
        result.size.height = ((max_y1>=max_y2)?max_y1:max_y2)-result.top_left.y+1;
	return result;
}

ei_rect_t intersect_clipper(ei_rect_t clipper1, ei_rect_t clipper2)
{
        ei_rect_t inter;
        /* On réordonne les clipper pour que x1 <= x2 */
        if (clipper1.top_left.x > clipper2.top_left.x) {
                inter = clipper1;
                clipper1 = clipper2;
                clipper2 = inter;
        }
        int x1, y1, w1, h1;
        int x2, y2, w2, h2;
        x1 = clipper1.top_left.x;
        x2 = clipper2.top_left.x;
        y1 = clipper1.top_left.y;
        y2 = clipper2.top_left.y;
        w1 = clipper1.size.width;
        w2 = clipper2.size.width;
        h1 = clipper1.size.height;
        h2 = clipper2.size.height;
        /* On teste si l'intersection est vide */
        if (x1+w1<=x2 || y2+h2<=y1 || y1+h1<=y2) {
                inter.top_left.x = 0; inter.top_left.y = 0;
                inter.size.width = 0; inter.size.height = 0;
                return inter;
        }
        /* Ici, on sait qu'il y a intersection */
        if (y1 <= y2) {
                /* y2 <= y1+h1 car intersection */
                inter.top_left.y = y2;
                inter.top_left.x = x2;
                if (x1+w1 <= x2+w2) {
                        inter.size.width = x1+w1-x2;
                }
                else {
                        inter.size.width = w2;
                }
                if (y2+h2 <= y1+h1) {
                        inter.size.height = h2;
                }
                else {
                        inter.size.height = y1+h1-y2;
                }
        }
        else {
                /* y2 + h2 > y1 car intersection */
                inter.top_left.x = x2;
                inter.top_left.y = y1;
                if (x1+w1 <= x2+w2) {
                        inter.size.width = x1+w1-x2;
                }
                else {
                        inter.size.width = w2;
                }
                if (y2+h2 <= y1+h1) {
                        inter.size.height = y2+h2-y1;
                }
                else {
                        inter.size.height = h1;
                }
        }
        return inter;
}

