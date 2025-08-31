#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdbool.h>
#include <stdio.h>

#include "src/core/init.h"

int main()
{
    int screen_width, screen_height;

    ALLEGRO_DISPLAY *display = init_allegro(&screen_width, &screen_height);

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_BITMAP *background = al_load_bitmap("assets/images/menu/background_menu.png");

    ALLEGRO_BITMAP *logo = al_load_bitmap("assets/images/logos/logo_only_title.png");

    ALLEGRO_BITMAP *play_game = al_load_bitmap("assets/images/buttons/play.png");

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_start_timer(timer);

    int btn_x = 810;
    int btn_y = 800;
    int btn_width = 300;
    int btn_height = 300;

    bool running = true;
    while (running)
    {
        ALLEGRO_EVENT event;
        while (al_get_next_event(event_queue, &event))
        {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = false;

            if (event.type == ALLEGRO_EVENT_KEY_DOWN)
                running = false;
        }

        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);

        al_draw_bitmap(logo, 450, -120, 0);

        al_draw_scaled_bitmap(play_game, 0, 0, al_get_bitmap_width(play_game),
                              al_get_bitmap_height(play_game), btn_x, btn_y, btn_width, btn_height,
                              0);

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            int mx = event.mouse.x;
            int my = event.mouse.y;

            if (mx >= btn_x && mx <= btn_x + btn_width && my >= btn_y && my <= btn_y + btn_height)
            {
                printf("Botao de jogar clicado!\n");
            }
        }

        al_flip_display();
    }

    al_destroy_bitmap(logo);
    al_destroy_bitmap(play_game);
    al_destroy_bitmap(background);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}
