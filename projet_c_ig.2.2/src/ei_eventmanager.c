#include "ei_eventmanager.h"
#include "ei_types.h"


ei_linked_tag_t *tags = NULL;
ei_linked_bind_t *binds_event[8];

void init_binds_event(ei_linked_tag_t ** binds)
{
        for (uint8_t i = 0; i<8; i++) {
                binds[i] = NULL;
        }
}

ei_linked_bind_t **get_binds_event()
{
        return binds_event;
}

// Fonction calculant renvoyant le widget correspondant à un pixel


// Fonction qui traite un évènement
ei_bool_t handle_event(ei_event_t* event)
{
        return EI_TRUE;
}


// Fonction traitante interne pour les boutons


// Fonction traitante interne pour les top_level


