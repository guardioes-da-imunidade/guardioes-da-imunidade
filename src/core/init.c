#include "init.h"

#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

ALLEGRO_DISPLAY* init_allegro(int* screen_width, int* screen_height)
{
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return NULL;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar o addon de imagens.\n");
        return NULL;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n");
        return NULL;
    }

    if (!al_init_font_addon())
    {
        fprintf(stderr, "Falha ao inicializar o addon de fontes.\n");
        return NULL;
    }

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar o addon TTF.\n");
        return NULL;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "Falha ao inicializar teclado.\n");
        return NULL;
    }

    if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar mouse.\n");
        return NULL;
    }

    if (!al_install_audio())
    {
        fprintf(stderr, "Falha ao inicializar áudio.\n");

        return NULL;
    }

    if (!al_init_acodec_addon())
    {
        fprintf(stderr, "Falha ao inicializar o addon de codecs de áudio.\n");

        return NULL;
    }

    *screen_width = 1280;
    *screen_height = 720;

    ALLEGRO_DISPLAY* display = al_create_display(*screen_width, *screen_height);
    if (!display)
    {
        fprintf(stderr, "Falha ao criar display.\n");
        return NULL;
    }

    al_set_window_title(display, "Guardiões da Imunidade");
    return display;
}
