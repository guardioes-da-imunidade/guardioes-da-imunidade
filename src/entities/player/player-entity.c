#include "player-entity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_player()
{
    if (PLAYER_ENTITY == NULL)
    {
        PLAYER_ENTITY = malloc(sizeof(PlayerEntity));
        if (PLAYER_ENTITY == NULL)
        {
            fprintf(stderr, "Falha ao alocar memória para PLAYER_ENTITY\n");
            exit(1);
        }
    }

    PLAYER_ENTITY->vaccines = 10;
    PLAYER_ENTITY->current_stage = 1;

    for (int i = 0; i < MAX_DEFENDERS_INVENTORY; i++)
    {
        PLAYER_ENTITY->defenders[i].defender_id = i;
        PLAYER_ENTITY->defenders[i].unlocked = false;
        snprintf(PLAYER_ENTITY->defenders[i].name, 32, "Defensor %d", i);
    }

    PLAYER_ENTITY->defenders[0].unlocked = true;
    strcpy(PLAYER_ENTITY->defenders[0].name, "Celula Branca");

    PLAYER_ENTITY->defenders[1].unlocked = true;
    strcpy(PLAYER_ENTITY->defenders[1].name, "Eosinofilo");

    PLAYER_ENTITY->defenders[2].unlocked = true;
    strcpy(PLAYER_ENTITY->defenders[2].name, "Basofilo");

    PLAYER_ENTITY->in_use_slots[0] = 0;
    PLAYER_ENTITY->in_use_slots[1] = 1;
    PLAYER_ENTITY->in_use_slots[2] = 2;
}
