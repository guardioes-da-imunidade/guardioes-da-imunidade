#include "sound_effect.h"

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


void play_sound_effect(ALLEGRO_SAMPLE *sample)
{
    if (sample)
    {
        al_play_sample(sample, 1.0 , 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    else
    {
        fprintf(stderr, "Falha ao reproduzir efeito sonoro. \n");
    }
}

