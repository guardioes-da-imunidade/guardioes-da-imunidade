#include "button.h"

void draw_button(Button *button)
{
    int mouse_x = get_mouse_x();
    int mouse_y = get_mouse_y();
    bool clicked = mouse_is_clicked();

    float button_right = button->x + button->width;
    float button_bottom = button->y + button->height;

    al_draw_filled_rectangle(button->x, button->y, button_right, button_bottom,
                             *button->fill_color);

    if (button->border.border_color)
    {
        float thickness = (button->border.thickness > 0) ? button->border.thickness : 4.f;
        al_draw_rectangle(button->x, button->y, button_right, button_bottom,
                          *button->border.border_color, thickness);
    }

    float text_x = button->x + button->width / 2;
    float text_y = button->y + button->height / 2 - 10;
    al_draw_text(button->text.font, *button->text.color, text_x, text_y, ALLEGRO_ALIGN_CENTER,
                 button->text.content);

    bool is_inside_x = mouse_x >= button->x && mouse_x <= button_right;
    bool is_inside_y = mouse_y >= button->y && mouse_y <= button_bottom;

    if (clicked && is_inside_x && is_inside_y && button->on_click)
        button->on_click(button->context);
}
