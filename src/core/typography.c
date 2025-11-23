#include "typography.h"

#include <allegro5/allegro.h>
#include <stdio.h>

ALLEGRO_FONT* fonts[FONT_COUNT] = {NULL};

void init_fonts()
{
    fonts[FONT_H1] = al_load_font(FONT_PATH_TITLE, 32, 0);
    fonts[FONT_H2] = al_load_font(FONT_PATH_TITLE, 28, 0);
    fonts[FONT_H3] = al_load_font(FONT_PATH_TITLE, 24, 0);
    fonts[FONT_H4] = al_load_font(FONT_PATH_TITLE, 20, 0);
    fonts[FONT_H5] = al_load_font(FONT_PATH_TITLE, 16, 0);
    fonts[FONT_H6] = al_load_font(FONT_PATH_TITLE, 12, 0);

    fonts[FONT_H7] = al_load_font(FONT_PATH_TITLE, 10, 0);
    fonts[FONT_H8] = al_load_font(FONT_PATH_TITLE, 8, 0);
    fonts[FONT_H9] = al_load_font(FONT_PATH_TITLE, 6, 0);


    for (int i = 0; i < FONT_COUNT; i++)
    {
        if (!fonts[i])
        {
            fprintf(stderr, "Erro ao carregar fonte %d\n.", i);
        }
    }
}

void destroy_fonts()
{
    for (int i = 0; i < FONT_COUNT; i++)
    {
        if (fonts[i])
        {
            al_destroy_font(fonts[i]);

            fonts[i] = NULL;
        }
    }
}
