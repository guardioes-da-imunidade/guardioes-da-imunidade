#include "./game_screen.h"

#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../base/menu.h"

extern GameState current_game_state;
extern Screen *current_screen;

static float square_x = 50;
static float square_y = 250;
static float circle_x = 800;
static float circle_y = 250;
static bool circle_active = true;
static float respawn_timer = 0;
static bool first_run = true;

static void init(ALLEGRO_DISPLAY *display)
{
    current_game_state = GAME_PLAYING;
    if (first_run)
    {
        srand(time(NULL));
        first_run = false;
    }
}

static void update(ALLEGRO_EVENT *event, bool *running)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        current_screen->destroy();
        current_screen = &MenuScreen;
        current_screen->init(NULL);
    }

    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        *running = false;
    }
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(20, 20, 40));

    square_y = (screen_height - 40) / 2;
    circle_y = square_y + 20;

    if (circle_active)
    {
        circle_x -= 2;
        if (circle_x < square_x + 40)
        {
            circle_active = false;
            respawn_timer = 120;
        }
    }

    if (!circle_active)
    {
        respawn_timer--;
        if (respawn_timer <= 0)
        {
            circle_active = true;
            circle_x = screen_width + 25;
            circle_y = square_y + 20;
        }
    }

    al_draw_filled_rectangle(square_x, square_y, square_x + 40, square_y + 40,
                             al_map_rgb(100, 150, 255));

    if (circle_active)
    {
        al_draw_filled_circle(circle_x, circle_y, 25, al_map_rgb(255, 100, 100));
    }
}

static void destroy(void) {}

Screen GameScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
