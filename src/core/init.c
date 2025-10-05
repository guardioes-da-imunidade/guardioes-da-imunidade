#include "init.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

ALLEGRO_DISPLAY *init_allegro(int *screen_width, int *screen_height)
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

    ALLEGRO_MONITOR_INFO context;
    al_get_monitor_info(0, &context);
    *screen_width = context.x2 - context.x1;
    *screen_height = context.y2 - context.y1;

    ALLEGRO_DISPLAY *display = al_create_display(*screen_width, *screen_height);

    if (!display)
    {
        fprintf(stderr, "Falha ao criar display.\n");

        return NULL;
    }

    return display;
}
