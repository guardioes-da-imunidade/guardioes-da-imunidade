#include "event.h"

void process_events(ALLEGRO_EVENT_QUEUE *queue, bool *running)
{
  ALLEGRO_EVENT event;

  while (al_get_next_event(queue, &event))
  {
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      *running = false;
    }

    if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      *running = false;
    }
  }
}
