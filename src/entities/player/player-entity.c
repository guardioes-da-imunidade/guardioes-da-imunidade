#include "player-entity.h"

#include <stdio.h>

void init_player()
{
    PLAYER_ENTITY->vaccines = 10;
    PLAYER_ENTITY->current_stage = 1;
}
