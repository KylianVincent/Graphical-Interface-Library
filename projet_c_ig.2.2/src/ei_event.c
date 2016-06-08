#include "ei_event.h"
#include "ei_eventmanager.h"

/* void		ei_bind			(ei_eventtype_t		eventtype, */
/* 					 ei_widget_t*		widget, */
/* 					 ei_tag_t		tag, */
/* 					 ei_callback_t		callback, */
/* 					 void*			user_param); */
void		ei_bind			(ei_eventtype_t		eventtype,
					 ei_widget_t*		widget,
					 ei_tag_t		tag,
					 ei_callback_t		callback,
					 void*			user_param)
{
        if (widget != NULL && tag != NULL){
                perror("Un bind ne peut être définit pour un widget et un bind en même temps.\n");
                exit(1);
        }
        ei_linked_bind_t *new_bind = calloc(1, sizeof(ei_linked_bind_t));
        new_bind->widget = widget;
        new_bind->tag = tag;
        new_bind->callback = callback;
        new_bind->user_param = user_param;
        ei_linked_bind_t **binds_event = get_binds_event();
        new_bind->next = binds_event[eventtype];
        binds_event[eventtype] = new_bind;
}

/* void		ei_unbind		(ei_eventtype_t		eventtype, */
/* 					 ei_widget_t*		widget, */
/* 					 ei_tag_t		tag, */
/* 					 ei_callback_t		callback, */
/* 					 void*			user_param); */
void		ei_unbind		(ei_eventtype_t		eventtype,
					 ei_widget_t*		widget,
					 ei_tag_t		tag,
					 ei_callback_t		callback,
					 void*			user_param)
{
        ei_linked_bind_t **binds_event = get_binds_event();
        ei_linked_bind_t *cour = binds_event[eventtype];
        ei_linked_bind_t *prec = cour;
        while (cour != NULL && (cour->widget != widget || cour->tag != tag 
				|| cour->callback != callback 
				|| cour->user_param != user_param)){
                prec = cour;
                cour = cour->next;
        }
        if (cour != NULL){
                if (prec == cour){
                        binds_event[eventtype] = cour->next;
                        free(cour);
                }else{
                        prec->next = cour->next;
                        free(cour);
                }
        }
}
