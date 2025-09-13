#include "player-entity.h"

#include <stdio.h>
#include <stdlib.h>

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
}
