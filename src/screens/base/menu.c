#include "menu.h"

#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "../../core/game.h"
#include "../game_screen/game_screen.h"
#include "config.h"

extern GameState current_game_state;
extern Screen *current_screen;

static ALLEGRO_BITMAP *background = NULL;
static ALLEGRO_BITMAP *logo = NULL;
static ALLEGRO_BITMAP *play_game = NULL;

static int btn_x = 810;
static int btn_y = 800;
static int btn_width = 300;
static int btn_height = 300;

static void init(ALLEGRO_DISPLAY *display)
{
    current_game_state = GAME_MENU;

    background = al_load_bitmap("assets/images/menu/background_menu.png");
    logo = al_load_bitmap("assets/images/logos/logo_only_title.png");
    play_game = al_load_bitmap("assets/images/buttons/play.png");
}

static void update(ALLEGRO_EVENT *event, bool *running)
{
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        if (mx >= btn_x && mx <= btn_x + btn_width && my >= btn_y && my <= btn_y + btn_height)
        {
            current_screen->destroy();

            current_screen = &GameScreen;
            current_screen->init(NULL);
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_C)
    {
        current_screen->destroy();

        current_screen = &ConfigScreen;
        current_screen->init(NULL);
    }
}

static void draw(int screen_width, int screen_height)
{
    if (background)
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);

    if (logo)
        al_draw_bitmap(logo, 450, -120, 0);

    if (play_game)
        al_draw_scaled_bitmap(play_game, 0, 0, al_get_bitmap_width(play_game),
                              al_get_bitmap_height(play_game), btn_x, btn_y, btn_width, btn_height,
                              0);
}

static void destroy(void)
{
    if (background)
        al_destroy_bitmap(background);
    if (logo)
        al_destroy_bitmap(logo);
    if (play_game)
        al_destroy_bitmap(play_game);
}

Screen MenuScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
