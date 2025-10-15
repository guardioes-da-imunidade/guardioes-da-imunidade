#ifndef BESTIARY_H
#define BESTIARY_H

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/palette.h"
#include "core/ui/button.h"
#include "screens/screen.h"

typedef struct Enemy
{
    const char *name;
    const char *description;
    const char *image_path;
    ALLEGRO_BITMAP *image;
} Enemy;

extern Screen Bestiary;

#endif
