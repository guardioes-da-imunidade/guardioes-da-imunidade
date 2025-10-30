#include "config.h"

#include <stdio.h>

#include "../../core/game.h"

extern GameState current_game_state;

static void init(ALLEGRO_DISPLAY *display)
{
    current_game_state = GAME_CONFIG;

    printf("Init ConfigScreen\n");
}

static void update(ALLEGRO_EVENT *event, bool *running) { printf("Update ConfigScreen\n"); }

static void draw(int screen_width, int screen_height) { printf("Draw ConfigScreen\n"); }

static void destroy(void) { printf("Destroy ConfigScreen\n"); }

Screen ConfigScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
