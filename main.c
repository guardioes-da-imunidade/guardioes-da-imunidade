#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdbool.h>
#include <stdio.h>

#include "src/core/game.h"
#include "src/core/init.h"
#include "src/screens/base/menu.h"

Screen *current_screen = &MenuScreen;

int main()
{
    int screen_width, screen_height;

    ALLEGRO_DISPLAY *display = init_allegro(&screen_width, &screen_height);

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_start_timer(timer);

    bool running = true;

    current_screen->init(display);

    while (running)
    {
        ALLEGRO_EVENT current_event;
        while (al_get_next_event(event_queue, &current_event))
        {
            current_screen->update(&current_event, &running);
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        current_screen->draw(screen_width, screen_height);
        al_flip_display();
    }

    current_screen->destroy();

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}
