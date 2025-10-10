#include "global_audio.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

ALLEGRO_BITMAP *audio_icon_off = NULL;
ALLEGRO_BITMAP *audio_icon_on = NULL;
ALLEGRO_BITMAP *increase_volume_icon = NULL;
ALLEGRO_BITMAP *decrease_volume_icon = NULL;
bool muted = false;

int audio_icons_x = 100;
int audio_icons_y = 20;
int audio_icons_width = 100;
int audio_icons_height = 100;

int volume_increase_icon_x = 220;
int volume_increase_icon_y = 20;
int volume_decrease_icon_x = 340;
int volume_decrease_icon_y = 20;
int volume_icons_width = 100;
int volume_icons_height = 100;

void load_audio_icons()
{
    audio_icon_off = al_load_bitmap("assets/images/buttons/audio_off.png");
    audio_icon_on = al_load_bitmap("assets/images/buttons/audio_on.png");
    increase_volume_icon = al_load_bitmap("assets/images/buttons/increase_volume_button.png");
    decrease_volume_icon = al_load_bitmap("assets/images/buttons/decrease_volume_button.png");

}

void draw_audio_icons()
{
    if (muted && audio_icon_off)
    {
        al_draw_scaled_bitmap(audio_icon_off, 0, 0, al_get_bitmap_width(audio_icon_off),
                              al_get_bitmap_height(audio_icon_off), audio_icons_x, audio_icons_y,
                              audio_icons_width, audio_icons_height, 0);
    }

    else if (!muted && audio_icon_on)
    {
        al_draw_scaled_bitmap(audio_icon_on, 0, 0, al_get_bitmap_width(audio_icon_on),
                              al_get_bitmap_height(audio_icon_on), audio_icons_x, audio_icons_y,
                              audio_icons_width, audio_icons_height, 0);
    }

    if (increase_volume_icon)
    {
        al_draw_scaled_bitmap(increase_volume_icon, 0, 0, al_get_bitmap_width(increase_volume_icon),
                              al_get_bitmap_height(increase_volume_icon), volume_increase_icon_x,
                              volume_increase_icon_y, volume_icons_width, volume_icons_height, 0);
    }
    if (decrease_volume_icon)
    {
        al_draw_scaled_bitmap(decrease_volume_icon, 0, 0, al_get_bitmap_width(decrease_volume_icon),
                              al_get_bitmap_height(decrease_volume_icon), volume_decrease_icon_x,
                              volume_decrease_icon_y, volume_icons_width, volume_icons_height, 0);
    }
}

void toggle_mute()
{
    ALLEGRO_MIXER *mixer = al_get_default_mixer();
    if (mixer)
    {
        int volume = al_get_mixer_gain(mixer);

        muted = !muted;
        if (muted)
        {
            al_set_mixer_gain(mixer, 0);
        }
        else
        {
            al_set_mixer_gain(mixer, 1);
        }
    }
}

void lower_volume()
{
    ALLEGRO_MIXER *mixer = al_get_default_mixer();
    if (mixer)
    {
        float volume = al_get_mixer_gain(mixer);
        volume -= 0.1;
        if (volume < 0)
        {
            volume = 0;
            muted = true;
        }
        else
        {
            muted = false;
        }
        al_set_mixer_gain(mixer, volume);
    }
}

void increase_volume()
{
    ALLEGRO_MIXER *mixer = al_get_default_mixer();
    if (mixer)
    {
        float volume = al_get_mixer_gain(mixer);
        volume += 0.1;
        if (volume > 1)
            volume = 1;

        al_set_mixer_gain(mixer, volume);

        if (volume == 0)
        {
            muted = true;
        }
        else
        {
            muted = false;
        }
    }
}
