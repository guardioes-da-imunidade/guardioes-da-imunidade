#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include <stdbool.h>

typedef struct PlayerEntity
{
    char defenders_unlocked[20];
    int current_stage;
    int vaccines;
} PlayerEntity;

extern PlayerEntity *PLAYER_ENTITY;

#endif
