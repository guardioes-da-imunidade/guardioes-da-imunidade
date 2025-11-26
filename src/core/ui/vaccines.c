#include "vaccines.h"

void draw_vaccines_count(int screen_width, int screen_height)
{
    (void)screen_height;

    int radius = 25;
    int circle_x = screen_width - radius - 10;
    int circle_y = radius + 10;
    al_draw_filled_circle(circle_x, circle_y, radius, COLOR_BLUE);
    al_draw_circle(circle_x, circle_y, radius, COLOR_BLACK, 3);

    char vaccines_text[10];
    snprintf(vaccines_text, sizeof(vaccines_text), "%d", Player->vaccines);
    al_draw_text(fonts[FONT_H5], COLOR_BLACK, circle_x, circle_y - 8, ALLEGRO_ALIGN_CENTRE,
                 vaccines_text);
}
