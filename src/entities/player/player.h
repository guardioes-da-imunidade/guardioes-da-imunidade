#ifndef PLAYER_H
#define PLAYER_H

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
    Defender* defenders[];
} PlayerEntity;

extern PlayerEntity* Player;

void init_player();

int get_player_equipped_inventory_size();

bool is_defender_unlocked(int id);
bool is_defender_equipped(int id);
const Defender* get_equipped_defender(int id);

void unlock_defender(Defender* defender);
void equip_defender(Defender* defender);
void unequip_defender(Defender* defender);

void print_slots();

#endif
