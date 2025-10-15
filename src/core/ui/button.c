#include "button.h"

void draw_button(Button *btn, int mouse_x, int mouse_y, bool mouse_clicked)
{
    al_draw_filled_rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height,
                             btn->text.fill_color);

    al_draw_rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height,
                      btn->text.border_color, 2);

    al_draw_text(btn->text.font, btn->text.color, btn->x + btn->width / 2,
                 btn->y + (btn->height / 2) - 10, ALLEGRO_ALIGN_CENTER, btn->text.content);

    bool is_intersecting_horizontal = mouse_x >= btn->x && mouse_x <= btn->x + btn->width;
    bool is_intersecting_vertical = mouse_y >= btn->y && mouse_y <= btn->y + btn->height;

    if (mouse_clicked && is_intersecting_horizontal && is_intersecting_vertical)
    {
        if (btn->on_click)
            btn->on_click();
    }
}
