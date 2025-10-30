#include "music.h"

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

ALLEGRO_AUDIO_STREAM *current_music = NULL;

void load_music(const char* filename)
{
    current_music = al_load_audio_stream(filename, 4, 4096);
    al_attach_audio_stream_to_mixer(current_music, al_get_default_mixer());
    al_set_audio_stream_playmode(current_music, ALLEGRO_PLAYMODE_LOOP);

    if (!current_music)
    {
        fprintf(stderr, "Falha ao carregar a música: %s\n", filename);
        return;
    }
}

void play_music()
{
    al_set_audio_stream_playing(current_music, true);
}

void stop_music()
{
    al_set_audio_stream_playing(current_music, false);
    al_rewind_audio_stream(current_music);
}

void destroy_music()
{
    if (current_music)
    {
        al_destroy_audio_stream(current_music);
        current_music = NULL;
    }
}
