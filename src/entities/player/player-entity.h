#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <stdbool.h>

#define MAX_DEFENDERS_INVENTORY 10
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
    DefenderInventory defenders[MAX_DEFENDERS_INVENTORY];
    int in_use_slots[MAX_IN_USE_SLOTS];
} PlayerEntity;

extern PlayerEntity* PLAYER_ENTITY;

void init_player();

#endif
