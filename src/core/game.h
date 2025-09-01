#ifndef GAME_H
#define GAME_H

typedef enum
{
    BOOTING,
    GAME_MENU,
    GAME_CONFIG,
    GAME_PLAY,
    GAME_PAUSE,
    GAME_OVER
} GameState;

extern GameState current_game_state;

#endif
