#include "player-entity.h"

void init_player()
{
    int count = get_immunecell_count();

    PLAYER_ENTITY = malloc(sizeof(PlayerEntity) + sizeof(Defender*) * count);

    if (PLAYER_ENTITY == NULL)
    {
        fprintf(stderr, "Falha ao alocar memória para PLAYER_ENTITY\n");
        exit(1);
    }

    PLAYER_ENTITY->vaccines = 10;
    PLAYER_ENTITY->current_stage = 1;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++) PLAYER_ENTITY->in_use_slots[i] = -1;

    for (int i = 0; i < count; i++) PLAYER_ENTITY->defenders[i] = NULL;

    const Defender* first = get_defender_by_id(0);
    if (first != NULL)
    {
        PLAYER_ENTITY->defenders[0] = (Defender*)first;
        PLAYER_ENTITY->in_use_slots[0] = 0;
    }
}

int get_player_equipped_inventory_size()
{
    int count = 0;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (PLAYER_ENTITY->in_use_slots[i] != -1)
            count++;

    return count;
}

bool is_defender_unlocked(int id) { return PLAYER_ENTITY->defenders[id] != NULL; }

bool is_defender_equipped(int id)
{
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (PLAYER_ENTITY->in_use_slots[i] == id)
            return true;

    return false;
}

// TODO: Da para melhorar porque em vários lugares acima do uso desse método eu tenho o mesmo for
// repetido
const Defender* get_equipped_defender(int id)
{
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (PLAYER_ENTITY->in_use_slots[i] == id)
            return get_defender_by_id(id);

    return NULL;
}

void unlock_defender(Defender* defender)
{
    int id = defender->id;

    if (is_defender_unlocked(id))
        return;

    if (PLAYER_ENTITY->vaccines >= defender->cost_to_unlock)
    {
        PLAYER_ENTITY->vaccines -= defender->cost_to_unlock;
        PLAYER_ENTITY->defenders[id] = defender;
    }
}

void equip_defender(Defender* defender)
{
    int id = defender->id;

    if (!is_defender_unlocked(id) || is_defender_equipped(id))
        return;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        if (PLAYER_ENTITY->in_use_slots[i] == -1)
        {
            PLAYER_ENTITY->in_use_slots[i] = id;

            return;
        }
    }
}

void unequip_defender(Defender* defender)
{
    int id = defender->id;

    if (!is_defender_equipped(id))
        return;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        if (PLAYER_ENTITY->in_use_slots[i] == id)
        {
            PLAYER_ENTITY->in_use_slots[i] = -1;

            return;
        }
    }
}

void print_slots()
{
    printf("[SLOTS] ");
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++) printf("%d ", PLAYER_ENTITY->in_use_slots[i]);
    printf("\n");
}
