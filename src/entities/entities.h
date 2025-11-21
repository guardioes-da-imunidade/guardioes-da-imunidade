#ifndef ENTITY_H
#define ENTITY_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

typedef enum
{
    PROJECTILE_WHITE_BALL = 0,
    PROJECTILE_BLUE_MAGIC = 1,
    PROJECTILE_RED_LASER = 2
} ProjectileType;

typedef struct
{
    const char* name;
    const char* description;
    int health;
    float speed;
    int attack;
    int defense;
    float attack_cooldown;
    bool active;
    int row;
    int col;
    int slot;
    float x;
    float y;
    ProjectileType projectile_type;
    const char* image_path;
    ALLEGRO_BITMAP* image;
    int image_width;
    int image_height;
} Entity;

typedef struct
{
    Entity base;
    int cost_to_place;
    int cost_to_unlock;
    int defender_id;
} ImmuneCell;

typedef struct
{
    Entity base;
    bool is_boss;
    int type;
} Pathogen;

typedef struct
{
    bool is_defender;
    Entity* entity;
} Microorganism;

extern Microorganism microorganisms[];
extern int total_microorganisms;

void load_microorganisms();

const ImmuneCell* get_immunecell_by_index(int index);

int get_immunecell_count();

#endif
