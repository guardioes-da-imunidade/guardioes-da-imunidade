#ifndef BESTIARY_H
#define BESTIARY_H

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/core/palette.h"
#include "src/core/ui/button.h"
#include "src/screens/screen.h"

typedef struct Enemy
{
    const char *name;
    const char *description;
    const char *image_path;
    ALLEGRO_BITMAP *image;
} Enemy;

extern Screen Bestiary;

#endif
