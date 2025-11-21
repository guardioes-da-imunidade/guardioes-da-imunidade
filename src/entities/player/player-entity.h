#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <stdbool.h>

#include "../entities.h"

#define MAX_IN_USE_SLOTS 3

typedef struct
{
    int defender_id;
    bool unlocked;
    char name[32];
} DefenderInventory;

typedef struct
{
    int vaccines;
    int current_stage;
    int in_use_slots[MAX_IN_USE_SLOTS];
    ImmuneCell* defenders[];
} PlayerEntity;

extern PlayerEntity* PLAYER_ENTITY;

void init_player();

int get_player_equipped_inventory_size();

bool is_defender_unlocked(int id);
bool is_defender_equipped(int id);
const ImmuneCell* get_equipped_defender(int id);

void unlock_defender(ImmuneCell* defender);
void equip_defender(ImmuneCell* defender);
void unequip_defender(ImmuneCell* defender);

#endif
