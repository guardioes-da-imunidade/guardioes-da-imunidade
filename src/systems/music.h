#ifndef MUSIC_H
#define MUSIC_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

void load_music(const char* filename);

void play_music(void);

void stop_music(void);

void destroy_music(void);

#endif
