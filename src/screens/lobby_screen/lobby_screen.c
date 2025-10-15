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

extern GameState current_game_state;
extern Screen *current_screen;

static ALLEGRO_BITMAP *background = NULL;
static ALLEGRO_FONT *font = NULL;
static bool first_run = true;

static void init(ALLEGRO_DISPLAY *display)
{
    current_game_state = GAME_PLAYING;

    if (first_run)
    {
        srand(time(NULL));
        first_run = false;
    }

    ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
    al_change_directory(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    al_destroy_path(path);

    background = al_load_bitmap("assets/images/menu/background_lobby.png");
    font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 18, 0);
    if (!font)
        font = al_create_builtin_font();
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
        return;
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        int col_x1 = 60;
        int col_y1 = 100;
        int col_x2 = 280;
        int col_y2 = 170;
        if (mx >= col_x1 && mx <= col_x2 && my >= col_y1 && my <= col_y2)
        {
            printf("Coleção\n");
        }

        int screen_width = al_get_display_width(al_get_current_display());
        int screen_height = al_get_display_height(al_get_current_display());

        int button_width = 250;
        int button_height = 80;
        int spacing = 60;
        int total_width = (button_width * 2) + spacing;
        int start_x = (screen_width - total_width) / 2;
        int y = screen_height - 250;

        int sf_x1 = start_x;
        int sf_y1 = y;
        int sf_x2 = sf_x1 + button_width;
        int sf_y2 = sf_y1 + button_height;

        int his_x1 = sf_x2 + spacing;
        int his_y1 = y;
        int his_x2 = his_x1 + button_width;
        int his_y2 = his_y1 + button_height;

        if (mx >= sf_x1 && mx <= sf_x2 && my >= sf_y1 && my <= sf_y2)
        {
            printf("Sem fim\n");
        }

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
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);

    ALLEGRO_COLOR yellow = al_map_rgb(255, 204, 0);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR blue = al_map_rgb(135, 206, 250);

    float col_x1 = 60, col_y1 = 200, col_x2 = 280, col_y2 = 270;
    al_draw_filled_rectangle(col_x1, col_y1, col_x2, col_y2, yellow);
    al_draw_rectangle(col_x1, col_y1, col_x2, col_y2, black, 4);
    al_draw_text(font, black, (col_x1 + col_x2) / 2, col_y1 + 25, ALLEGRO_ALIGN_CENTRE, "Coleção");

    float button_width = 250;
    float button_height = 80;
    float spacing = 60;

    float total_width = (button_width * 2) + spacing;
    float start_x = (screen_width - total_width) / 2;
    float y = screen_height - 150;

    float sf_x1 = start_x;
    float sf_y1 = y;
    float sf_x2 = sf_x1 + button_width;
    float sf_y2 = sf_y1 + button_height;

    float his_x1 = sf_x2 + spacing;
    float his_y1 = y;
    float his_x2 = his_x1 + button_width;
    float his_y2 = his_y1 + button_height;

    al_draw_filled_rectangle(sf_x1, sf_y1, sf_x2, sf_y2, yellow);
    al_draw_rectangle(sf_x1, sf_y1, sf_x2, sf_y2, black, 4);
    al_draw_text(font, black, (sf_x1 + sf_x2) / 2, sf_y1 + 30, ALLEGRO_ALIGN_CENTRE, "Sem fim");

    al_draw_filled_rectangle(his_x1, his_y1, his_x2, his_y2, yellow);
    al_draw_rectangle(his_x1, his_y1, his_x2, his_y2, black, 4);
    al_draw_text(font, black, (his_x1 + his_x2) / 2, his_y1 + 30, ALLEGRO_ALIGN_CENTRE, "História");

    float circle_x = screen_width - 80;
    float circle_y = 220;
    float radius = 40;
    al_draw_filled_circle(circle_x, circle_y, radius, blue);
    al_draw_circle(circle_x, circle_y, radius, black, 4);

    char vaccines_text[10];
    snprintf(vaccines_text, sizeof(vaccines_text), "%d", PLAYER_ENTITY->vaccines);
    al_draw_text(font, black, circle_x, circle_y - 7, ALLEGRO_ALIGN_CENTRE, vaccines_text);
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
