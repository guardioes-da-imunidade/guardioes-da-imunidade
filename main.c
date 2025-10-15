#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdbool.h>
#include <stdio.h>

#include "src/core/game.h"
#include "src/core/init.h"
#include "src/core/input.h"
#include "src/core/palette.h"
#include "src/entities/player/player-entity.h"
#include "src/screens/base/menu.h"

Screen *current_screen = &MenuScreen;
PlayerEntity *PLAYER_ENTITY = NULL;

int main()
{
    int screen_width, screen_height;

    ALLEGRO_DISPLAY *display = init_allegro(&screen_width, &screen_height);

    init_colors();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_start_timer(timer);

    current_screen->init(display);

    bool running = true;

    init_player();

    while (running)
    {
        ALLEGRO_EVENT event;
        while (al_get_next_event(event_queue, &event))
        {
            input_update(&event);
            current_screen->update(&event, &running);
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        current_screen->draw(screen_width, screen_height);
        al_flip_display();

        reset_mouse_click();
    }

    current_screen->destroy();

    free(PLAYER_ENTITY);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}
