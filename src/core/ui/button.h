#ifndef BUTTON_H
#define BUTTON_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

typedef void (*OnClickCallback)(void);

typedef struct
{
    const char *content;
    ALLEGRO_COLOR color;
    ALLEGRO_COLOR fill_color;
    ALLEGRO_COLOR border_color;
    ALLEGRO_FONT *font;
} ButtonText;

typedef struct
{
    float x,y,width,height;
    ButtonText text;
    OnClickCallback on_click;
} Button;

void draw_button(Button *btn, int mouse_x, int mouse_y, bool mouse_clicked);

#endif
