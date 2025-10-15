#ifndef GLOBAL_AUDIO_H
#define GLOBAL_AUDIO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

extern int audio_icons_x;
extern int audio_icons_y;
extern int audio_icons_width;
extern int audio_icons_height;

extern int volume_increase_icon_x;
extern int volume_increase_icon_y;
extern int volume_decrease_icon_x;
extern int volume_decrease_icon_y;
extern int volume_icons_width;
extern int volume_icons_height;

void load_audio_icons(void);

void draw_audio_icons(void);

void toggle_mute(void);

void lower_volume(void);

void increase_volume(void);

#endif
