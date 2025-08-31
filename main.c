#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "src/core/event.h"
#include "src/core/init.h"
#include "src/systems/resource.h"

int main()
{
    int screen_width, screen_height;

    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY *display = init_allegro(&screen_width, &screen_height);
    if (!display)
        return 1;

    al_set_window_title(display, "Guardiões da imunidade");

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_BITMAP *logo = load_bitmap_centered("assets/logos/logo.png", display);
    if (!logo)
        return 1;

    ALLEGRO_FONT *font = al_load_ttf_font("assets/fonts/arial.ttf", 32, 0);

    int logo_width = al_get_bitmap_width(logo);
    int logo_height = al_get_bitmap_height(logo);
    float logo_x_position = (screen_width - logo_width) / 2.0f;
    float logo_y_position = (screen_height - logo_height) / 2.0f;

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    bool running = true;
    while (running)
    {
        process_events(event_queue, &running);

        al_clear_to_color(al_map_rgb(251, 247, 238));
        al_draw_bitmap(logo, logo_x_position, logo_y_position, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), 650, 1000, 0,
                     "Aperte qualquer tecla para começar!");
        al_flip_display();
    }

    al_destroy_font(font);
    al_destroy_bitmap(logo);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
