#include "player-entity.h"

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

    const ImmuneCell* first = get_immunecell_by_index(0);
    if (first != NULL)
    {
        strncpy(PLAYER_ENTITY->defenders[0].name, first->base.name,
                sizeof(PLAYER_ENTITY->defenders[0].name) - 1);
        PLAYER_ENTITY->defenders[0].name[sizeof(PLAYER_ENTITY->defenders[0].name) - 1] = '\0';

        PLAYER_ENTITY->defenders[0].unlocked = true;
    }
}
