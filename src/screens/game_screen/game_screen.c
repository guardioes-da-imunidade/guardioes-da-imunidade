#include "game_screen.h"

#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#include "../../core/game.h"
#include "../base/menu.h"

extern GameState current_game_state;
extern Screen *current_screen;

static void init(ALLEGRO_DISPLAY *display) { current_game_state = GAME_PLAYING; }

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

    int square_size = 200;
    int square_x = (screen_width - square_size) / 2;
    int square_y = (screen_height - square_size) / 2;

    al_draw_filled_rectangle(square_x, square_y, square_x + square_size, square_y + square_size,
                             al_map_rgb(100, 150, 255));

    al_draw_rectangle(square_x, square_y, square_x + square_size, square_y + square_size,
                      al_map_rgb(255, 255, 255), 3);

    // TODO: Essa é a tela onde vai continuar, desenhei um quadrado e apliquei algumas cores além de
    // outros detalhes, mas tudo isso você pode modificar, qualquer dúvida mande no PR.
}

static void destroy(void) {}

Screen GameScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
