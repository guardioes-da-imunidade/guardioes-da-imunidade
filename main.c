#include <allegro5/allegro5.h>

#include "src/core/event.h"
#include "src/core/init.h"
#include "src/systems/resource.h"

int main()
{
    int screen_width, screen_height;

    ALLEGRO_DISPLAY *display = init_allegro(&screen_width, &screen_height);
    if (!display)
        return 1;

    ALLEGRO_BITMAP *logo = load_bitmap_centered("assets/logos/logo.png", display);
    if (!logo)
        return 1;

    int logo_width = al_get_bitmap_width(logo);
    int logo_height = al_get_bitmap_height(logo);
    float logo_x_position = (screen_width - logo_width) / 2.00f;
    float logo_y_position = (screen_height - logo_height) / 2.00f;

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    bool running = true;
    while (running)
    {
        process_events(event_queue, &running);

        al_clear_to_color(al_map_rgb(251, 247, 238));
        al_draw_bitmap(logo, logo_x_position, logo_y_position, 0);
        al_flip_display();
    }

    al_destroy_bitmap(logo);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
