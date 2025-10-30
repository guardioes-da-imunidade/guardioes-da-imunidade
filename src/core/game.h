#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro5.h>
#include <stdbool.h>

typedef enum
{
    GAME_MENU,
    GAME_CONFIG,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

extern GameState current_game_state;

#endif
