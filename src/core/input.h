#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include <stdio.h>

void input_update(ALLEGRO_EVENT *event);

int get_mouse_x();
int get_mouse_y();
bool mouse_is_down();
bool mouse_is_clicked();
void reset_mouse_click();

#endif
