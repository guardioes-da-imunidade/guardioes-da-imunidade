#include "./endless_mode_screen.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../base/menu.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* background = NULL;

#define GRID_COLS 9
#define GRID_ROWS 5
#define GRID_START_Y 120

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_PLAYING;

    background = al_load_bitmap("assets/images/maps/initial.png");
    if (!background)
    {
        fprintf(stderr, "Erro ao carregar imagem de fundo!\n");
    }
}

static void update(ALLEGRO_EVENT* event, bool* running)
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

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        if (mouse_y < GRID_START_Y)
            return;

        float cell_width = (float)al_get_display_width(al_get_current_display()) / GRID_COLS;
        float cell_height =
            (float)(al_get_display_height(al_get_current_display()) - GRID_START_Y) / GRID_ROWS;

        int col = (int)(mouse_x / cell_width);
        int row = (int)((mouse_y - GRID_START_Y) / cell_height);

        if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS)
        {
            printf("Clicou no quadrado [%d][%d]\n", row, col);
        }
    }
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (background)
    {
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);

        float cell_width = ((float)screen_width / GRID_COLS) - 5.0f;
        float cell_height = (float)(screen_height - GRID_START_Y) / GRID_ROWS;

        for (int row = 0; row < GRID_ROWS; row++)
        {
            for (int col = 0; col < GRID_COLS; col++)
            {
                float x = col * (cell_width + 1.0f);
                float y = GRID_START_Y + row * cell_height;

                al_draw_rectangle(x, y, x + cell_width, (y + cell_height) - 10,
                                  al_map_rgba(255, 0, 0, 150), 2.0f);
            }
        }
    }
}

static void destroy(void)
{
    if (background)
    {
        al_destroy_bitmap(background);
        background = NULL;
    }
}

Screen EndlessModeScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
