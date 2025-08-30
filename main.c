#include <allegro5/allegro.h>
#include <stdio.h>

int main()
{
  if (!al_init())
  {
    fprintf(stderr, "Falha ao inicializar Allegro.\n");

    return 1;
  }

  ALLEGRO_MONITOR_INFO context;
  al_get_monitor_info(0, &context);
  int screen_width = context.x2 - context.x1;
  int screen_height = context.y2 - context.y1;

  ALLEGRO_DISPLAY *display = al_create_display(screen_width, screen_height);

  if (!display)
  {
    fprintf(stderr, "Falha ao criar display.\n");

    return 1;
  }

  while (true)
  {
  }

  return 0;
}
