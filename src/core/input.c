#include "input.h"

static int mouse_x = 0;
static int mouse_y = 0;
static bool mouse_down = false;
static bool mouse_clicked = false;

void input_update(ALLEGRO_EVENT *event)
{
    switch (event->type)
    {
        case ALLEGRO_EVENT_MOUSE_AXES:
            mouse_x = event->mouse.x;
            mouse_y = event->mouse.y;

            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            mouse_down = true;
            mouse_clicked = true;

            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            mouse_down = false;

            break;

        default:
            break;
    }
}

int get_mouse_x() { return mouse_x; }

int get_mouse_y() { return mouse_y; }

bool mouse_is_down() { return mouse_down; }

bool mouse_is_clicked()
{
    bool clicked = mouse_clicked;

    mouse_clicked = false;

    return clicked;
}
