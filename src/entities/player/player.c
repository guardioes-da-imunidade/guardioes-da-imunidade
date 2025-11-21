#include "player.h"

void init_player()
{
    int count = get_defenders_count();

    Player = malloc(sizeof(PlayerEntity) + sizeof(Defender*) * count);

    if (Player == NULL)
    {
        fprintf(stderr, "Falha ao alocar memória para Player\n");
        exit(1);
    }

    Player->vaccines = 10;
    Player->current_stage = 1;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++) Player->in_use_slots[i] = -1;

    for (int i = 0; i < count; i++) Player->defenders[i] = NULL;

    const Defender* first = get_defender_by_id(0);
    if (first != NULL)
    {
        Player->defenders[0] = (Defender*)first;
        Player->in_use_slots[0] = 0;
    }
}

int get_player_equipped_inventory_size()
{
    int count = 0;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (Player->in_use_slots[i] != -1)
            count++;

    return count;
}

bool is_defender_unlocked(int id) { return Player->defenders[id] != NULL; }

bool is_defender_equipped(int id)
{
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (Player->in_use_slots[i] == id)
            return true;

    return false;
}

// TODO: Da para melhorar porque em vários lugares acima do uso desse método eu tenho o mesmo for
// repetido
const Defender* get_equipped_defender(int id)
{
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        if (Player->in_use_slots[i] == id)
            return get_defender_by_id(id);

    return NULL;
}

void unlock_defender(Defender* defender)
{
    int id = defender->base.id;

    if (is_defender_unlocked(id))
        return;

    if (Player->vaccines >= defender->cost_to_unlock)
    {
        Player->vaccines -= defender->cost_to_unlock;
        Player->defenders[id] = defender;
    }
}

void equip_defender(Defender* defender)
{
    int id = defender->base.id;

    if (!is_defender_unlocked(id) || is_defender_equipped(id))
        return;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        if (Player->in_use_slots[i] == -1)
        {
            Player->in_use_slots[i] = id;

            return;
        }
    }
}

void unequip_defender(Defender* defender)
{
    int id = defender->base.id;

    if (!is_defender_equipped(id))
        return;

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        if (Player->in_use_slots[i] == id)
        {
            Player->in_use_slots[i] = -1;

            return;
        }
    }
}

void print_slots()
{
    printf("[SLOTS] ");
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++) printf("%d ", Player->in_use_slots[i]);
    printf("\n");
}
