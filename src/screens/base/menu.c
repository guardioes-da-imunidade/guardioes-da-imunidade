#include "menu.h"

#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "../../core/game.h"
#include "../../systems/global_audio.h"
#include "../../systems/music.h"
#include "../../systems/sound_effect.h"
#include "../lobby_screen/lobby_screen.h"
#include "config.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_BITMAP* logo = NULL;
static ALLEGRO_BITMAP* play_game = NULL;
static ALLEGRO_SAMPLE* button_clicked = NULL;

static int btn_x = 540;
static int btn_y = 500;
static int btn_width = 200;
static int btn_height = 180;

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_MENU;
    al_reserve_samples(1);

    background = al_load_bitmap("assets/images/menu/background_menu.png");
    logo = al_load_bitmap("assets/images/logos/logo_only_title.png");
    play_game = al_load_bitmap("assets/images/buttons/play.png");
    load_audio_icons();
    button_clicked = al_load_sample("assets/audios/sound_effects/play_game.wav");
    load_music("assets/audios/musics/background_music.wav");
    play_music();
}

static void update(ALLEGRO_EVENT* event, bool* running)
{
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        if (mx >= audio_icons_x && mx <= audio_icons_x + audio_icons_width && my >= audio_icons_y &&
            my <= audio_icons_y + audio_icons_height)
        {
            toggle_mute();
        }
        else if (mx >= volume_increase_icon_x &&
                 mx <= volume_icons_width + volume_increase_icon_x &&
                 my >= volume_increase_icon_y && my <= volume_increase_icon_y + volume_icons_height)
        {
            increase_volume();
        }
        else if (mx >= volume_decrease_icon_x &&
                 mx <= volume_decrease_icon_x + volume_icons_width &&
                 my >= volume_decrease_icon_y && my <= volume_decrease_icon_y + volume_icons_height)
        {
            lower_volume();
        }

        if (mx >= btn_x && mx <= btn_x + btn_width && my >= btn_y && my <= btn_y + btn_height)
        {
            stop_music();
            play_sound_effect(button_clicked);
            al_rest(1.5);

            current_screen->destroy();

            current_screen = &LobbyScreen;
            current_screen->init(NULL);
        }
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
                              al_get_bitmap_height(background), 0, 0, 1280, 720, 0);

    if (logo)
    {
        int logo_width = al_get_bitmap_width(logo) / 1.5;
        int logo_height = al_get_bitmap_height(logo) / 1.5;
        al_draw_scaled_bitmap(logo, 0, 0, al_get_bitmap_width(logo), al_get_bitmap_height(logo),
                              640 - (logo_width / 2), -100, logo_width, logo_height, 0);
    }

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
