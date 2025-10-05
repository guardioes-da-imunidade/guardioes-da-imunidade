#ifndef SOUND_EFFECT_H
#define SOUND_EFFECT_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

void init_sound_effect(int reserved_samples);

ALLEGRO_SAMPLE* load_sound_effect(const char *file_path);

void play_sound_effect(ALLEGRO_SAMPLE *sample, float volume);

#endif
