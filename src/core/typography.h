#ifndef TYPOGRAPHY_H
#define TYPOGRAPHY_H

#include <allegro5/allegro_font.h>

#define FONT_PATH_TITLE "assets/fonts/PressStart2P.ttf"

typedef enum
{
    FONT_H1,
    FONT_H2,
    FONT_H3,
    FONT_H4,
    FONT_H5,
    FONT_H6,
    FONT_H7,
    FONT_H8,
    FONT_H9,

    FONT_COUNT
} FontType;

extern ALLEGRO_FONT* fonts[FONT_COUNT];

void init_fonts();

void destroy_fonts();

#endif
