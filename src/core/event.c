#include "event.h"

void process_events(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *event, bool *running)
{
    while (al_get_next_event(queue, event))
    {
        if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            *running = false;
        }

        if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            *running = false;
        }
    }
}
