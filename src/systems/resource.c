#include "resource.h"

#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>

ALLEGRO_BITMAP *load_bitmap_centered(const char *path, ALLEGRO_DISPLAY *display)
{
  ALLEGRO_BITMAP *image = al_load_bitmap(path);

  if (!image)
  {
    al_show_native_message_box(display, "Erro", "Carregamento de imagem", path, NULL,
                               ALLEGRO_MESSAGEBOX_ERROR);

    return NULL;
  }

  return image;
}
