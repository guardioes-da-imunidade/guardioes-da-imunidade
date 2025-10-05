#include "sound_effect.h"

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

void init_sound_effect(int reserved_sample)
{
    if (!al_reserve_samples(reserved_sample))
    {
        fprintf(stderr, "Falha ao reservar os canais de áudio.\n");

        return;
    }
}

ALLEGRO_SAMPLE* load_sound_effect(const char *file_path)
{
    ALLEGRO_SAMPLE *sample = al_load_sample(file_path);
    if (!sample)
    {
        fprintf(stderr, "Falha ao carregar efeito sonoro: %s\n", file_path);
    }
    return sample;
}

void play_sound_effect(ALLEGRO_SAMPLE *sample, float volume)
{
    if (sample)
    {
        al_play_sample(sample, volume, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    else
    {
        fprintf(stderr, "Falha ao reproduzir efeito sonoro. \n");
    }
}

