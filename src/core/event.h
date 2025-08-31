#ifndef EVENT_H
#define EVENT_H

#include <allegro5/allegro.h>
#include <stdbool.h>

void process_events(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *event, bool *running);

#endif
