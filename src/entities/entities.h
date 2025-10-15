#ifndef ENTITY_H
#define ENTITY_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

typedef struct
{
    const char *name;
    const char *description;
    int health;
    float speed;
    int attack;
    int defense;
    float attack_cooldown;
    const char *image_path;
    ALLEGRO_BITMAP *image;
} Entity;

typedef struct
{
    Entity base;
    float cost_to_place;
} Defender;

typedef struct
{
    Entity base;
    bool is_boss;
} Enemy;

typedef struct
{
    bool is_defender;
    Entity *entity;
} Microorganism;

extern Microorganism microorganisms[];
extern int total_microorganisms;

void load_microorganisms();

#endif
