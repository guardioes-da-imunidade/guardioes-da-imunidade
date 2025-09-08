#ifndef ENEMY_H
#define ENEMY_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>


typedef struct Enemy{
    int positionX;
    int positionY;
    int width;
    int height;
    int speed;
    ALLEGRO_BITMAP* enemy_image(width, height);
}Enemy

void init_enemy(Enemy* e, int positionX, int positionY, int width, int height, int speed, const char* image_path);
void update_enemy(Enemy*e);
void draw_enemy(const Enemy*e);

#endif