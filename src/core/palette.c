#include "palette.h"

ALLEGRO_COLOR COLOR_BLACK;
ALLEGRO_COLOR COLOR_WHITE;
ALLEGRO_COLOR COLOR_YELLOW;
ALLEGRO_COLOR COLOR_TRANSPARENT;

void init_colors()
{
    COLOR_BLACK = al_map_rgb(0, 0, 0);
    COLOR_WHITE = al_map_rgb(255, 255, 255);
    COLOR_YELLOW = al_map_rgb(255, 204, 0);
    COLOR_TRANSPARENT = al_map_rgba(0, 0, 0, 0);
}
