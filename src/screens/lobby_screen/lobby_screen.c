#include "./lobby_screen.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../../entities/player/player-entity.h"
#include "../base/menu.h"
#include "../endless_mode_screen/endless_mode_screen.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_FONT* font = NULL;
static bool first_run = true;

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_PLAYING;

    if (first_run)
    {
        srand(time(NULL));
        first_run = false;
    }

    ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
    al_change_directory(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    al_destroy_path(path);

    background = al_load_bitmap("assets/images/menu/background_lobby.png");
    font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 14, 0);
    if (!font)
        font = al_create_builtin_font();
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
        return;
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        int col_x1 = 1050;
        int col_y1 = 170;
        int col_x2 = 1220;
        int col_y2 = 260;
        if (mx >= col_x1 && mx <= col_x2 && my >= col_y1 && my <= col_y2)
        {
            printf("Coleção\n");
        }

        int sf_x1 = 100;
        int sf_y1 = 400;
        int sf_x2 = 280;
        int sf_y2 = 510;
        if (mx >= sf_x1 && mx <= sf_x2 && my >= sf_y1 && my <= sf_y2)
        {
            current_screen->destroy();
            current_screen = &EndlessModeScreen;
            current_screen->init(NULL);
        }

        int his_x1 = 360;
        int his_y1 = 130;
        int his_x2 = 950;
        int his_y2 = 460;
        if (mx >= his_x1 && mx <= his_x2 && my >= his_y1 && my <= his_y2)
        {
            printf("História\n");
        }
    }
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(255, 255, 255));

    if (background)
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, 1280, 720, 0);

    ALLEGRO_COLOR blue = al_map_rgb(135, 206, 250);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR dark_overlay = al_map_rgba(0, 0, 0, 180);

    float circle_x = 1240;
    float circle_y = 50;
    float radius = 25;
    al_draw_filled_circle(circle_x, circle_y, radius, blue);
    al_draw_circle(circle_x, circle_y, radius, black, 3);

    char vaccines_text[10];
    snprintf(vaccines_text, sizeof(vaccines_text), "%d", PLAYER_ENTITY->vaccines);
    al_draw_text(font, black, circle_x, circle_y - 8, ALLEGRO_ALIGN_CENTRE, vaccines_text);
}

static void destroy(void)
{
    if (background)
        al_destroy_bitmap(background);
    if (font)
        al_destroy_font(font);
}

Screen LobbyScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
