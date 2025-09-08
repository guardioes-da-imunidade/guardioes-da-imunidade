#include "enemy.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>


void init_enemy(Enemy* e, int positionX, int positionY, int width, int height, int speed, const char* image_path){
    e->positionX = positionX;
    e->positionY = positionY;
    e->width = width;
    e->height = height;
    e->speed = speed;
    e->enemy_image = al_load_bitmap(image_path);
    if (!e->enemy_image) 
    {
        printf("Erro ao carregar imagem do inimigo: %s\n", image_path);
    }
}

void update_enemy(Enemy* e){
    e->positionX -= e-> speed;
}

void draw_enemy(const Enemy* e){
    if (e->enemy_image) {
        al_draw_bitmap(e->enemy_image, e->positionX, e->positionY, 0);
    } 
}