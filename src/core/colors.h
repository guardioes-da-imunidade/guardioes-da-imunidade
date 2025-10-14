#ifndef COLORS_H
#define COLORS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>

static inline ALLEGRO_COLOR COLOR_BLACK() { return al_map_rgb(0, 0, 0); }
static inline ALLEGRO_COLOR COLOR_WHITE() { return al_map_rgb(255, 255, 255); }
static inline ALLEGRO_COLOR COLOR_TRANSPARENT() { return al_map_rgba(0, 0, 0, 0); }

#endif
