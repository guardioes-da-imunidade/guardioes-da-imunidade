#ifndef SCREEN_H
#define SCREEN_H

#include <allegro5/allegro.h>
#include <stdbool.h>

typedef struct Screen
{
    // Inicialização
    void (*init)(ALLEGRO_DISPLAY *display);

    // Lógica interna da Tela, modifica o estado atual
    void (*update)(ALLEGRO_EVENT *event, bool *running);

    // Lógica interna da Tela, reage ao estado atual
    void (*draw)(int screen_width, int screen_height);

    // Limpa recursos
    void (*destroy)(void);
} Screen;

#endif
