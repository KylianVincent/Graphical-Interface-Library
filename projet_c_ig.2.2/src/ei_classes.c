#include "ei_classes.h"
#include "ei_globs.h"
#include <math.h>
#include <string.h>

/*-------FONCTIONS AUXILIAIRES-------------*/

void free_linked_point(ei_linked_point_t* pts);

/* Trace un bouton complet ou seulement la partie haut ou basse */
/* mode = 0 : complet  //  mode = -1 : basse  //  mode = 1 : haute  */
ei_linked_point_t* rounded_frame(ei_rect_t rect, int radius, int8_t mode);


/* ----------FRAME------------*/

void* frame_allocfunc(){

	ei_frame_t *frame = calloc(1, sizeof(ei_frame_t));
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
                if (frame->img_rect != NULL) {
                        free(frame->img_rect);
                }
                if (frame->text != NULL) {
                        free(frame->text);
                }
		if (widget->content_rect != &(widget->screen_location)){
			free(widget->content_rect);
		}
                free(frame);
        }
}


void frame_drawfunc(struct ei_widget_t*	frame,
		    ei_surface_t		surface,
		    ei_surface_t		pick_surface,
		    ei_rect_t*		clipper){ 
	
	/*création de la surface principale*/

	ei_linked_point_t *cadre=rounded_frame(*(frame->content_rect),0,0);

	/*On trace le cadre en relief*/

	hw_surface_lock(surface);
	hw_surface_lock(pick_surface);

	if (((ei_frame_t*)frame)->border_width != 0){
			
		ei_linked_point_t *cadre_inferieur_bas=rounded_frame(frame->screen_location,0,-1);
		ei_linked_point_t *cadre_inferieur_haut=rounded_frame(frame->screen_location,0,1);
		
		/*determination de la couleur de tracé*/

		int8_t signe;
		if (((ei_frame_t*)frame)->relief ==1){
			signe = 1;
		}else if  (((ei_frame_t*)frame)->relief ==2){
			signe= -1;
		}else{
			signe=0;
		}
		ei_color_t color_sup= eclaircir_assombrir(((ei_frame_t*)frame)->color,50,signe);
		ei_color_t color_inf=eclaircir_assombrir(((ei_frame_t*)frame)->color,50,-signe);


		ei_draw_polygon(surface, cadre_inferieur_bas, color_inf, clipper);
		ei_draw_polygon(surface, cadre_inferieur_haut, color_sup, clipper);
		ei_draw_polygon(pick_surface, cadre_inferieur_bas,*(frame->pick_color), clipper);
		ei_draw_polygon(pick_surface, cadre_inferieur_haut,*(frame->pick_color), clipper);
                /* On libère les listes chainées */
                free_linked_point(cadre_inferieur_bas);
                free_linked_point(cadre_inferieur_haut);
	}

	ei_draw_polygon(surface, cadre, ((ei_frame_t *)frame)->color, clipper);
        ei_draw_polygon(pick_surface, cadre, *(frame->pick_color), clipper);
        /* On libere les listes chainées */
        free_linked_point(cadre);
	
        ei_rect_t inter = intersect_clipper(*(frame->content_rect),*clipper);
        draw_texte(frame, surface, &inter);
        draw_img(frame, surface, &inter);

	hw_surface_unlock(surface);
	hw_surface_unlock(pick_surface);
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
        if (widget != NULL) {
                free(widget->pick_color);
                ei_button_t* button = (ei_button_t*) widget;
                if (button->img_rect != NULL) {
                        free(button->img_rect);
                }
                if (button->text != NULL) {
                        free(button->text);
                }
		if (widget->content_rect != &(widget->screen_location)){
			free(widget->content_rect);
		}
                free(button);
        }
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
        ei_linked_point_t* relief_sup = rounded_frame(widget->screen_location, button->corner_radius, 1);
        ei_linked_point_t* relief_inf = rounded_frame(widget->screen_location, button->corner_radius, -1);
        int rayon_corps = button->corner_radius - button->border_width;
        if (rayon_corps<0) {
                rayon_corps = 0;
        }
        ei_linked_point_t* corps = rounded_frame(*(widget->content_rect), rayon_corps, 0);
        /* Calcule si le button est enfoncé ou non */
        int8_t signe;
        if (button->relief ==1){
                signe = 1;
        }else if  (button->relief ==2){
                signe= -1;
        }else{
                signe=0;
        }

        /* On trace les surfaces correspondantes */
        hw_surface_lock(surface);
        hw_surface_lock(pick_surface);
        ei_draw_polygon(surface, relief_sup, eclaircir_assombrir(button->color,100,signe),  clipper);
        ei_draw_polygon(surface, relief_inf, eclaircir_assombrir(button->color,100,-signe),  clipper);
        ei_draw_polygon(surface, corps, button->color,  clipper);
        /* On trace aussi dans pick_surface */
        ei_draw_polygon(pick_surface, relief_inf, *(widget->pick_color),  clipper);
        ei_draw_polygon(pick_surface, relief_sup, *(widget->pick_color),  clipper);
        /* On affiche le texte et les images */
        ei_rect_t inter = intersect_clipper(*(widget->content_rect),*clipper);
        draw_texte(widget, surface, &inter);
        draw_img(widget, surface, &inter);

        hw_surface_unlock(surface);
        hw_surface_unlock(pick_surface);
        /* On libère les listes de points */
        free_linked_point(corps);
        free_linked_point(relief_sup);
        free_linked_point(relief_inf);
}


void button_setdefaultsfunc(struct ei_widget_t* widget)
{
        ei_button_t* button  = (ei_button_t*) widget;
        /* On définit une taille par défaut du bouton */
        widget->requested_size.width = 20;
        widget->requested_size.height = 20;
        widget->screen_location.size.width = 20;
        widget->screen_location.size.height = 20;
	button->color = ei_default_background_color;
	button->border_width = k_default_button_border_width;
	/* On change le content rect vu que border_width != 0 */
	widget->content_rect = calloc(1, sizeof(ei_rect_t));
	widget->content_rect->top_left.x = widget->screen_location.top_left.x + button->border_width;
	widget->content_rect->top_left.y = widget->screen_location.top_left.y + button->border_width;
	widget->content_rect->size.width = widget->screen_location.size.width - button->border_width * 2;
	widget->content_rect->size.height = widget->screen_location.size.height - button->border_width * 2;
        button->corner_radius = k_default_button_corner_radius;
	button->relief = ei_relief_raised;
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



/* --------------TOPLEVEL-------------- */
void toplevel_geomnotifyfunc (struct ei_widget_t *widget, ei_rect_t rect){
        if (widget != NULL){
                if (widget->content_rect == &(widget->screen_location)){
                        /* Le content_rect pointe vers la screen_location, il
                           faut allouer la place nécessaire au content_rect */
                        widget->content_rect = calloc(1, sizeof(ei_rect_t));
                }
        ei_toplevel_t* toplevel  = (ei_toplevel_t*) widget;
        widget->content_rect->top_left.x = widget->screen_location.top_left.x
                + toplevel->border_width;
        widget->content_rect->top_left.y = widget->screen_location.top_left.y
                + toplevel->height_header;
        widget->content_rect->size.width = widget->screen_location.size.width
                - 2*toplevel->border_width;
        widget->content_rect->size.height = widget->screen_location.size.height
                - toplevel->border_width - toplevel->height_header;
        }
}

void* toplevel_allocfunc (){
        ei_toplevel_t* toplevel = calloc(1, sizeof(ei_toplevel_t));
        if (toplevel == NULL){
                perror("Impossible d'allouer l'espace pour une toplevel.\n");
                exit(1);
        }
        return (void*) toplevel;
}

void toplevel_releasefunc (struct ei_widget_t* widget){
        if (widget != NULL){
                free(widget->pick_color);
                ei_toplevel_t* toplevel  = (ei_toplevel_t*) widget;

                if (toplevel->min_size != NULL){
                        free(toplevel->min_size);
                }
                if (toplevel->title != NULL) {
                        free(toplevel->title);
                }
                if (widget->content_rect != &(widget->screen_location)){
			free(widget->content_rect);
		}
                free(toplevel);
                widget = NULL;
        }
}

void toplevel_drawfunc(struct ei_widget_t* widget,
		     ei_surface_t		surface,
		     ei_surface_t		pick_surface,
                       ei_rect_t*		clipper){
        ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
        
        /* Calcul de la géométrie du cadre extérieur */
        int radius_header = toplevel->height_header/2;

        ei_point_t centre1 = ei_point(widget->screen_location.top_left.x + radius_header,
                                      widget->screen_location.top_left.y + radius_header);
        ei_point_t centre2 = ei_point(widget->screen_location.top_left.x + widget->screen_location.size.width - radius_header,
                                      widget->screen_location.top_left.y + radius_header);
        ei_linked_point_t *exterior = NULL;
        ei_linked_point_t *cour = NULL;
        cour = create_arc(centre1, radius_header, M_PI, 3.0*M_PI/2, &exterior);
        cour = create_arc(centre2, radius_header, 3.0*M_PI/2, 2*M_PI, &(cour->next));
        cour->next = calloc(1, sizeof(ei_linked_point_t));
        if (cour->next == NULL) {
                        perror("Impossible d'allouer l'espace mémoire nécessaire.\n");
                        exit(1);
                }
        cour->next->point = ei_point(widget->screen_location.top_left.x + widget->screen_location.size.width,
                                    widget->screen_location.top_left.y + widget->screen_location.size.height);
        cour = cour->next;
        cour->next = calloc(1, sizeof(ei_linked_point_t));
        if (cour->next == NULL) {
                        perror("Impossible d'allouer l'espace mémoire nécessaire.\n");
                        exit(1);
                }
        cour->next->point = ei_point(widget->screen_location.top_left.x,
                                    widget->screen_location.top_left.y + widget->screen_location.size.height);

        /* Calcul de la géométrie de la zone de texte */
        ei_point_t title_top_left = ei_point(widget->screen_location.top_left.x + 2 * radius_header,
                                             widget->screen_location.top_left.y);

        ei_linked_point_t *interior = rounded_frame(*(widget->content_rect), 0, 0);
        

        /*Tracé des surfaces */
	hw_surface_lock(surface);
	hw_surface_lock(pick_surface);
        ei_draw_polygon(surface, exterior, eclaircir_assombrir(toplevel->color, 100, -1), clipper);
        ei_draw_polygon(surface, interior, toplevel->color, clipper);
        ei_draw_polygon(pick_surface, exterior, *(widget->pick_color), clipper);

        /* On affiche le titre dans l'en_tête */
        ei_color_t white = {0xFF, 0xFF, 0xFF, 0xFF};
        ei_draw_text(surface,
                     &(title_top_left),
                     toplevel->title,
                     NULL,
                     &white,
                     &(widget->screen_location));
        
        hw_surface_unlock(surface);
        hw_surface_unlock(pick_surface);

        /* On libère les listes de points */
        free_linked_point(exterior);
        free_linked_point(interior);
        

}

void toplevel_setdefaultsfunc(struct ei_widget_t* widget){
        ei_toplevel_t* toplevel = (ei_toplevel_t *) widget;
        
        toplevel->color = ei_default_background_color;
        widget->requested_size = ei_size(320, 240);
        toplevel->border_width = 4;
        /* Taille maximale d'un titre = 50 char */
        toplevel->title = calloc(50, sizeof(char));
        strcpy(toplevel->title, "Toplevel");
        toplevel->closable = EI_TRUE;
        toplevel->resizable = ei_axis_both;
        toplevel->min_size = calloc(1, sizeof(ei_size_t));
        if (toplevel->min_size == NULL){
                perror("Allocation de mémoire pour la taille minimum de la toplevel impossible.\n");
                exit(1);
        }
        *(toplevel->min_size) = ei_size(160, 120);

        ei_size_t title_size;
        hw_text_compute_size(toplevel->title,
                             ei_default_font,
                             &(title_size.width),
                             &(title_size.height));
        toplevel->height_header = title_size.height;

        /* On change le content rect pour une toplevel */
	widget->content_rect = calloc(1, sizeof(ei_rect_t));
	widget->content_rect->top_left.x = widget->screen_location.top_left.x + toplevel->border_width;
	widget->content_rect->top_left.y = widget->screen_location.top_left.y + toplevel->height_header;
	widget->content_rect->size.width = widget->requested_size.width;
	widget->content_rect->size.height = widget->requested_size.height;
        widget->screen_location.size.width = widget->requested_size.width + toplevel->border_width * 2;
        widget->screen_location.size.height = widget->requested_size.height + toplevel->height_header + toplevel->border_width;
}
