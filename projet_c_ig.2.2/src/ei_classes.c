#include "ei_classes.h"
#include <math.h>
/*-------FONCTIONS AUXILIAIRES-------------*/

ei_point_t calcul_point_ancrage(struct ei_widget_t* widget, ei_anchor_t *anchor){

	/*simplification expressions*/

	int x=widget->screen_location.top_left.x; 
	int y=widget->screen_location.top_left.y;

	int width=(widget->screen_location.size).width;
	int height=(widget->screen_location.size).height;

	ei_point_t ancrage;

	if (anchor != NULL){
                switch (*anchor) {

                case ei_anc_center :
			ancrage.x= x + width/2; 
			ancrage.y= y + height/2;
                        break;

                case ei_anc_north :
			ancrage.x= x + width/2; 
			ancrage.y= y;
                        break;

                case ei_anc_northeast :
			ancrage.x= x + width; 
			ancrage.y= y;
                        break;

                case ei_anc_east :
			ancrage.x= x + width; 
			ancrage.y= y + height/2;
                        break;

                case ei_anc_southeast :
			ancrage.x= x + width; 
			ancrage.y= y + height;
                        break;

                case ei_anc_south :
			ancrage.x= x + width/2; 
			ancrage.y= y + height;
                        break;

                case ei_anc_southwest :
			ancrage.x= x; 
			ancrage.y= y + height;
                        break;

                case ei_anc_west :
			ancrage.x= x; 
			ancrage.y= y +height/2;
                        break;

                case ei_anc_northwest :
			ancrage.x= x; 
			ancrage.y= y;
                        break;

                case ei_anc_none :
			/*cas par défault northwest*/
			ancrage.x= x; 
			ancrage.y= y;
                        break;
                }
        }else{
		/*cas par défault northwest*/
		ancrage.x= x; 
		ancrage.y= y;
	}
	return ancrage;

}

/* ----------FRAME------------*/

void* frame_allocfunc(){

	struct ei_frame_t *frame = calloc(1, sizeof(struct ei_frame_t));
	if (frame != NULL){
		return (void*) frame;	
	}
	perror("Erreur allocation frame. \n");
	exit(1);
}


void frame_releasefunc(struct ei_widget_t* widget){ 

        if (widget != NULL)
        {
                free(widget->pick_color);
                ei_frame_t* frame  = (ei_frame_t*) widget; /*on caste le widget pour acceder à toute la zone mémoire*/
                free(frame);
        }
}

void frame_drawfunc(struct ei_widget_t*	frame,
		    ei_surface_t		surface,
		    ei_surface_t		pick_surface,
		    ei_rect_t*		clipper){

	int x=frame->screen_location.top_left.x; /*Origine du dessin*/
	int y=frame->screen_location.top_left.y;

	/*simplification expressions*/

	int b= ((ei_frame_t*)frame)->border_width; 

	int width=(frame->screen_location.size).width;
	int height=(frame->screen_location.size).height;

	ei_linked_point_t points[4];

/*création du cadre principal en teant compte des bordures*/
	int coords[]={x+b, y+b, 
		      x+width-b, y+b,
		      x+width-b, y+height-b,
		      x+b, y+height-b};
		      
	for (int i = 0; i < 8; i+=2) {
		points[i/2].point.x	= coords[i];
		points[i/2].point.y	= coords[i + 1];

		if (i < 6){
			points[i/2].next	= &(points[i/2+1]);
		}else{
			points[i/2].next	= NULL;	
		}

		/*blocage de la surface pour le dessin des triangles*/
		hw_surface_lock(surface);
		hw_surface_lock(pick_surface);
	}

/*traitement du relief*/

	if ((((ei_frame_t*)frame)->relief ==1 ||((ei_frame_t*)frame)->relief ==2)  && b!=0){

		ei_linked_point_t points_triangle1[4];
		ei_linked_point_t points_triangle2[4];
		
		/*création des deux tringles créant l'imptression de relief*/
		int coords_triangle1[]={x, y, 
					x+width, y,
					x, y+height};

		int coords_triangle2[]={x+width, y,
					x+width, y+height,
					x, y+height};

		/*création de la liste chainée de points*/
		for (int i = 0; i < 6; i+=2) {

			points_triangle1[i/2].point.x	= coords_triangle1[i];
			points_triangle1[i/2].point.y	= coords_triangle1[i + 1];

			points_triangle2[i/2].point.x	= coords_triangle2[i];
			points_triangle2[i/2].point.y	= coords_triangle2[i + 1];

			if (i < 4){
				points_triangle1[i/2].next	= &(points_triangle1[i/2+1]);
				points_triangle2[i/2].next	= &(points_triangle2[i/2+1]);
			}else{
				points_triangle1[i/2].next	= NULL;	
				points_triangle2[i/2].next	= NULL;	
			

			}		
		}
		ei_color_t color_triangle1=((ei_frame_t*)frame)->color;
		ei_color_t color_triangle2=((ei_frame_t*)frame)->color;
		/*delta permet de modifier uniformément la couleyr des triangles*/
		unsigned char delta;
		
		if (((ei_frame_t*)frame)->relief ==1){

			delta =100; /*cas above*/
		}else{
			delta=-100; /*cas under*/
		}

		color_triangle1.red +=delta %255;
		color_triangle1.green += delta %255;
		color_triangle1.blue += delta %255;

		color_triangle2.red -=delta %255;
		color_triangle2.green -=delta %255;
		color_triangle2.blue -= delta %255;

		/*dessin des deux triangles*/
		ei_draw_polygon(surface, points_triangle1, color_triangle1, clipper);
		ei_draw_polygon(surface, points_triangle2, color_triangle2, clipper);

	}


	ei_draw_polygon(surface, points, ((ei_frame_t*) frame)->color, clipper);
	
	/*gestion de l'affichage du texte*/
	
	if (((ei_frame_t*) frame)->text != NULL){

		ei_point_t ancrage= calcul_point_ancrage(frame, &(((ei_frame_t*) frame)->text_anchor));

		ei_draw_text(surface,
			     &(ancrage),
			     ((ei_frame_t*) frame)->text,
			     ((ei_frame_t*) frame)->text_font,
			     &(((ei_frame_t*) frame)->text_color),
			     clipper);

	 /*gestion de l'affichage d'une image*/	
	}else if (((ei_frame_t*) frame)->img != NULL){
		printf("img \n");

		ei_rect_t rect_dest;
		rect_dest.top_left= calcul_point_ancrage(frame, &(((ei_frame_t*) frame)->img_anchor));
		rect_dest.size.width=(((ei_frame_t*)frame)->img_rect)->size.width;
		rect_dest.size.width=(((ei_frame_t*)frame))->img_rect->size.height;

		int img_OK=ei_copy_surface(surface,
					   &rect_dest,
					   ((ei_frame_t*) frame)->img,
					   ((ei_frame_t*) frame)->img_rect,
					   EI_FALSE); /*pas sur !!!!!!!*/
		if (img_OK == 1){
			perror("problème d'insertion d'image");
			exit(1);
		}


	}
	
	ei_draw_polygon(pick_surface, points, *(frame->pick_color), clipper);

	hw_surface_unlock(surface);
	hw_surface_unlock(pick_surface);

	hw_surface_update_rects(surface, NULL);
	hw_surface_update_rects(pick_surface, NULL);

		 

}

void frame_setdefaultsfunc(struct ei_widget_t* widget){
	ei_frame_t* frame  = (ei_frame_t*) widget; /*on caste le widget pour acceder à toute la zone mémoire*/
	frame->color= ei_default_background_color;
	frame->border_width=0;
	frame->relief=ei_relief_none;
	frame->text=NULL;
	frame->text_font=ei_default_font;
	frame->text_color=ei_font_default_color;
	frame->text_anchor=ei_anc_center;
	frame->img=NULL;
	frame->img_rect=NULL;
	frame->img_anchor=ei_anc_center;
}


/* --------------BUTTON-------------- */

void* button_allocfunc ()
{
        ei_button_t* button = calloc(1, sizeof(ei_button_t));
        if (button == NULL) {
                perror("Impossible d'allouer l'espace pour un bouton.\n");
                exit(1);
        }
        return (void*) button;
}

void button_releasefunc (struct ei_widget_t* widget)
{
        free(widget->pick_color);
        ei_button_t* button = (ei_button_t*) widget;
        free(button);
}

/* Trace un arc de points avec c:centre / r:rayon / a1:angle de début / a2:angle de fin */
/* Requiert a1 < a2, r >= 0, et tail != NULL */
/* Insert l'arc à la suite de head (change sa valeur) */
/* Renvoie la queue de liste */
ei_linked_point_t* create_arc(ei_point_t c, uint32_t r, float a1, float a2, ei_linked_point_t** head)
{
        *head = calloc(1, sizeof(ei_linked_point_t));
        if (*head == NULL) {
                perror("Impossible d'allouer l'espace nécessaire.\n");
                exit(1);
        }
        ei_linked_point_t* cour = *head;
        cour->point.x = round(r*cos(a1))+c.x;
        cour->point.y = round(r*sin(a1))+c.y;
        if (r > 0)
        {
                const float pas = 2.0;
                int k = 1;
                while (a1+(float) k*pas/r < a2) {
                        cour->next = calloc(1, sizeof(ei_linked_point_t));
                        if (cour->next == NULL) {
                                perror("Impossible d'allouer l'espace nécessaire.\n");
                                exit(1);
                        }
                        cour = cour->next;
                        cour->point.x = round(r*cos(a1+(float)k*pas/r))+c.x;
                        cour->point.y = round(r*sin(a1+(float)k*pas/r))+c.y;
                        k++;
                }
                cour->next = calloc(1, sizeof(ei_linked_point_t));
                if (cour->next == NULL) {
                        perror("Impossible d'allouer l'espace nécessaire.\n");
                        exit(1);
                }
                cour = cour->next;
                cour->point.x = round(r*cos(a2))+c.x;
                cour->point.y = round(r*sin(a2))+c.y;
        }
        return cour;
}

void free_linked_point(ei_linked_point_t* pts)
{
        while (pts != NULL) 
        {
                ei_linked_point_t* temp = pts;
                pts = pts->next;
                free(temp);
        }
}

/* Trace un bouton complet ou seulement la partie haut ou basse */
/* mode = 0 : complet  //  mode = -1 : basse  //  mode = 1 : haute  */
ei_linked_point_t* rounded_frame(ei_rect_t rect, int radius, int8_t mode)
{
        ei_linked_point_t* pts = NULL;
        ei_linked_point_t* cour = NULL;
        if (mode >= 0)
        {
                ei_point_t c;
                c.x = rect.top_left.x + radius;
                c.y = rect.top_left.y + rect.size.height - radius;
                cour = create_arc(c, radius, 3.0*M_PI/4.0, M_PI, &pts);
                c.y = rect.top_left.y + radius;
                cour = create_arc(c, radius, M_PI, 3.0*M_PI/2.0, &(cour->next));
                c.x = rect.top_left.x + rect.size.width - radius;
                cour = create_arc(c, radius, 3.0*M_PI/2.0, 7.0*M_PI/4.0, &(cour->next));
        }
        if (mode <= 0)
        {
                ei_point_t c;
                c.x = rect.top_left.x + rect.size.width - radius;
                c.y = rect.top_left.y + radius;
                if (pts == NULL) {
                        cour = create_arc(c, radius, 7.0*M_PI/4.0, 2.0*M_PI, &pts);
                }
                else {
                        cour = create_arc(c, radius, 7.0*M_PI/4.0, 2.0*M_PI, &(cour->next));
                }
                c.y = rect.top_left.y + rect.size.height - radius;
                cour = create_arc(c, radius, 0, M_PI/2.0, &(cour->next));
                c.x = rect.top_left.x + radius;
                cour = create_arc(c, radius, M_PI/2.0, 3.0*M_PI/4.0, &(cour->next));
        }
        if (mode != 0) {
                ei_linked_point_t* h1 = calloc(1, sizeof(ei_linked_point_t));
                ei_linked_point_t* h2 = calloc(1, sizeof(ei_linked_point_t));
                if (rect.size.width >= rect.size.height) {
                        h1->point.y = rect.top_left.y + rect.size.height/2;
                        h1->point.x = rect.top_left.x + rect.size.height/2;
                        h2->point.y = h1->point.y;
                        h2->point.x = rect.top_left.x + rect.size.width - rect.size.height/2;
                }
                else {
                        h2->point.x = rect.top_left.x + rect.size.width/2;
                        h2->point.y = rect.top_left.y + rect.size.width/2;
                        h1->point.x = h2->point.x;
                        h1->point.y = rect.top_left.y + rect.size.height - rect.size.width/2;
                }
                if (mode > 0) {
                        cour->next = h2;
                        h2->next = h1;
                }
                else {
                        cour->next = h1;
                        h1->next = h2;
                }
        }
        return pts;
}

/* Change la couleur sur chaque octet de val */
/* Eclairit ou assombrit en fonction de sign (1 ou -1) */
ei_color_t eclaircir_assombrir(ei_color_t color, uint8_t val, int8_t sign)
{
        int16_t r = color.red + sign*val;
        int16_t g = color.green + sign*val;
        int16_t b = color.blue + sign*val;
        r = (r>255)?255:r;
        color.red = (r<0)?0:r;
        g = (g>255)?255:g;
        color.green = (g<0)?0:g;
        b = (b>255)?255:b;
        color.blue = (b<0)?0:b;
        return color;
}

void button_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
		     ei_rect_t*		clipper)
{
        /* On initialise les listes de points */
        ei_button_t* button = (ei_button_t*) widget;
        ei_linked_point_t* corps = rounded_frame(widget->screen_location, button->corner_radius, 0);
        ei_rect_t rect_border = widget->screen_location;
        rect_border.top_left.x -= button->border_width;
        rect_border.top_left.y -= button->border_width;
        rect_border.size.height += button->border_width * 2;
        rect_border.size.width += button->border_width * 2;
        int rayon_border = (button->corner_radius > 0)?button->corner_radius+button->border_width:0;
        ei_linked_point_t* relief_sup = rounded_frame(rect_border, rayon_border, 1);
        ei_linked_point_t* relief_inf = rounded_frame(rect_border, rayon_border, -1);
        /* On trace les surfaces correspondantes */
        hw_surface_lock(surface);
        hw_surface_lock(pick_surface);
        ei_draw_polygon(surface, relief_sup, eclaircir_assombrir(button->color,100,1),  clipper);
        ei_draw_polygon(pick_surface, relief_sup, *(widget->pick_color),  clipper);
        ei_draw_polygon(surface, relief_inf, eclaircir_assombrir(button->color,100,-1),  clipper);
        ei_draw_polygon(pick_surface, relief_inf, *(widget->pick_color),  clipper);
        ei_draw_polygon(surface, corps, button->color,  clipper);
        hw_surface_unlock(surface);
        hw_surface_unlock(pick_surface);
        hw_surface_update_rects(surface, NULL);
        hw_surface_update_rects(pick_surface, NULL);
        /* On libère les listes de points */
        free_linked_point(corps);
        free_linked_point(relief_sup);
        free_linked_point(relief_inf);
}


void button_setdefaultsfunc(struct ei_widget_t* widget)
{
        ei_button_t* button  = (ei_button_t*) widget;
	button->color = ei_default_background_color;
	button->border_width = k_default_button_border_width;
        button->corner_radius = k_default_button_corner_radius;
	button->relief = ei_relief_none;
	button->text = NULL;
	button->text_font = ei_default_font;
	button->text_color = ei_font_default_color;
	button->text_anchor = ei_anc_center;
	button->img = NULL;
	button->img_rect = NULL;
	button->img_anchor = ei_anc_center;
        button->callback = NULL;
        button->user_param = NULL;
}
