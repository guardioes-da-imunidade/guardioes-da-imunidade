#ifndef BUTTON_H
#define BUTTON_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>

#include "core/input.h"

typedef void (*OnClickCallback)(void);

typedef struct
{
    const char *content;
    const ALLEGRO_COLOR *color;
    ALLEGRO_FONT *font;
} ButtonText;

typedef struct
{
    const ALLEGRO_COLOR *border_color;
    float thickness;
} ButtonBorder;

typedef struct
{
    float x, y, width, height;
    const ALLEGRO_COLOR *fill_color;
    ButtonText text;
    ButtonBorder border;
    OnClickCallback on_click;
} Button;

void draw_button(Button *button);

#endif
