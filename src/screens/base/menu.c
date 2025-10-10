#include "menu.h"

#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "../../systems/sound_effect.h"
#include "../../systems/music.h"
#include "../../systems/global_audio.h"
#include "../../core/game.h"
#include "../game_screen/game_screen.h"
#include "config.h"

extern GameState current_game_state;
extern Screen *current_screen;

static ALLEGRO_BITMAP *background = NULL;
static ALLEGRO_BITMAP *logo = NULL;
static ALLEGRO_BITMAP *play_game = NULL;
static ALLEGRO_SAMPLE *button_clicked = NULL;

static int btn_x = 810;
static int btn_y = 800;
static int btn_width = 300;
static int btn_height = 300;

static void init(ALLEGRO_DISPLAY *display)
{
    current_game_state = GAME_MENU;
    init_sound_effect(1);

    background = al_load_bitmap("assets/images/menu/background_menu.png");
    logo = al_load_bitmap("assets/images/logos/logo_only_title.png");
    play_game = al_load_bitmap("assets/images/buttons/play.png");
    load_audio_icons();
    button_clicked = load_sound_effect("assets/audios/play_game.wav");
    load_music("assets/audios/background_music_test.wav");
    play_music();
}

static void update(ALLEGRO_EVENT *event, bool *running)
{
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        if (mx >= audio_icons_x && mx <= audio_icons_x + audio_icons_width && my >= audio_icons_y && my <= audio_icons_y + audio_icons_height)
        {
            toggle_mute();
        }
        else if (mx >= volume_increase_icon_x  && mx <= volume_icons_width + volume_increase_icon_x && my >= volume_increase_icon_y && my <= volume_increase_icon_y + volume_icons_height)
        {
            increase_volume();
        }
        else if (mx >= volume_decrease_icon_x && mx <= volume_decrease_icon_x + volume_icons_width && my >= volume_decrease_icon_y && my <= volume_decrease_icon_y + volume_icons_height)
        {
            lower_volume();
        }

        if (mx >= btn_x && mx <= btn_x + btn_width && my >= btn_y && my <= btn_y + btn_height)
        {
            stop_music();
            play_sound_effect(button_clicked, 5.0);
            al_rest(1.5);

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

    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        *running = false;
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
    draw_audio_icons();
}

static void destroy(void)
{
    if (background)
        al_destroy_bitmap(background);
    if (logo)
        al_destroy_bitmap(logo);
    if (play_game)
        al_destroy_bitmap(play_game);
    if (button_clicked)
        al_destroy_sample(button_clicked);
        destroy_music();
}

Screen MenuScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
