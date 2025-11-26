#include "./lobby_screen.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../../entities/player/player.h"
#include "../base/menu.h"
#include "../boss_stage_screen/boss_stage_screen.h"
#include "../endless_mode_screen/endless_mode_screen.h"
#include "../stage_screen/stage_screen.h"

extern GameState current_game_state;
extern Screen* current_screen;

typedef struct
{
    const char* image_path;
    const char* text;
} CutsceneData;

typedef struct
{
    float x;
    float y;
    int level_number;
    bool is_special;
} LevelNode;

static CutsceneData cutscenes[] = {
    {"assets/images/scene/scene_planet_earth.png",
     "No início, o mundo parecia normal — até o dia em que um novo vírus misterioso surgiu.\n"
     "Ninguém sabia de onde veio. Alguns diziam ser um acidente de laboratório, outros, uma "
     "criação\n"
     "que saiu do controle. O que ninguém esperava era que ninguém escaparia:\n"
     "cada ser humano acabou infectado."},
    {"assets/images/scene/scene_danger_alert.png",
     "Dentro de cada humano em seu sistema imunológico, os alarmes foram ativados.\n"
     "As defesas sentiram a presença de algo estranho se espalhando.\n"
     "Os defensores entenderam o que aquilo significava:\n"
     "não era uma simples infecção — era o início de uma guerra."},
    {"assets/images/scene/virus_attack_scene.png",
     "Do caos contra a saúde, surgiu o inimigo. Um vírus capaz de criar e controlar legiões "
     "inteiras\n"
     "de bactérias mutantes. Agora, a sobrevivência depende dos defensores do corpo,\n"
     "que precisam reagir e evoluir para enfrentar essa nova ameaça.\n"
     "A batalha pela vida está prestes a começar."}};

static const int TOTAL_CUTSCENES = 3;
static const int TOTAL_LEVELS = 5;
static const float NODE_RADIUS = 40.0f;
static const float BOSS_NODE_RADIUS = 55.0f;

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_BITMAP* background_history = NULL;
static ALLEGRO_FONT* font = NULL;
static ALLEGRO_FONT* cutscene_font = NULL;
static ALLEGRO_FONT* level_font = NULL;
static ALLEGRO_FONT* boss_font = NULL;
static bool first_run = true;

static bool showing_cutscene = false;
static bool in_history_screen = false;
static bool cutscene_already_seen = false;
static int current_cutscene_index = 0;
static ALLEGRO_BITMAP* cutscene_images[3] = {NULL, NULL, NULL};

static LevelNode level_nodes[5];
static double blink_timer = 0.0;
static double pulse_timer = 0.0;

static bool show_tutorial_modal = false;

static void init_level_nodes(void)
{
    float center_x = 1280.0f / 2.0f;
    float start_y = 180.0f;
    float spacing_y = 100.0f;

    level_nodes[0].x = center_x - 200.0f;
    level_nodes[0].y = start_y;
    level_nodes[0].level_number = 1;
    level_nodes[0].is_special = false;

    level_nodes[1].x = center_x + 200.0f;
    level_nodes[1].y = start_y + spacing_y;
    level_nodes[1].level_number = 2;
    level_nodes[1].is_special = false;

    level_nodes[2].x = center_x - 200.0f;
    level_nodes[2].y = start_y + spacing_y * 2;
    level_nodes[2].level_number = 3;
    level_nodes[2].is_special = false;

    level_nodes[3].x = center_x + 200.0f;
    level_nodes[3].y = start_y + spacing_y * 3;
    level_nodes[3].level_number = 4;
    level_nodes[3].is_special = false;

    level_nodes[4].x = center_x;
    level_nodes[4].y = start_y + spacing_y * 4 + 20.0f;
    level_nodes[4].level_number = 5;
    level_nodes[4].is_special = true;
}

static void init(ALLEGRO_DISPLAY* display)
{
    (void)display;

    current_game_state = GAME_PLAYING;

    if (first_run)
    {
        srand(time(NULL));
        first_run = false;
    }

    show_tutorial_modal = false;

    ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
    al_change_directory(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    al_destroy_path(path);

    background = al_load_bitmap("assets/images/menu/background_lobby.png");
    background_history = al_load_bitmap("assets/images/menu/background_lobby_history.png");
    font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 14, 0);
    cutscene_font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 16, 0);
    level_font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 24, 0);
    boss_font = al_load_ttf_font("assets/fonts/PressStart2P.ttf", 18, 0);

    if (!font)
        font = al_create_builtin_font();
    if (!cutscene_font)
        cutscene_font = al_create_builtin_font();
    if (!level_font)
        level_font = al_create_builtin_font();
    if (!boss_font)
        boss_font = al_create_builtin_font();

    cutscene_images[0] = al_load_bitmap("assets/images/scene/scene_planet_earth.png");
    cutscene_images[1] = al_load_bitmap("assets/images/scene/scene_danger_alert.png");
    cutscene_images[2] = al_load_bitmap("assets/images/scene/virus_attack_scene.png");

    init_level_nodes();
}

static void draw_level_connections(void)
{
    ALLEGRO_COLOR connection_color = al_map_rgb(80, 80, 80);
    float line_thickness = 6.0f;

    al_draw_line(level_nodes[0].x, level_nodes[0].y, level_nodes[1].x, level_nodes[1].y,
                 connection_color, line_thickness);

    al_draw_line(level_nodes[1].x, level_nodes[1].y, level_nodes[2].x, level_nodes[2].y,
                 connection_color, line_thickness);

    al_draw_line(level_nodes[2].x, level_nodes[2].y, level_nodes[3].x, level_nodes[3].y,
                 connection_color, line_thickness);

    float boss_glow = (sin(pulse_timer * 2.0) + 1.0) / 2.0;
    ALLEGRO_COLOR boss_line_color = al_map_rgb(150 + (int)(50 * boss_glow), 50, 50);

    al_draw_line(level_nodes[3].x, level_nodes[3].y, level_nodes[4].x, level_nodes[4].y,
                 boss_line_color, line_thickness);
}

static bool is_level_unlocked(int level_number) { return level_number <= Player->current_stage; }

static void draw_level_nodes(void)
{
    blink_timer += 0.05;
    pulse_timer += 0.05;

    float blink_factor = (sin(blink_timer * 3.0) + 1.0) / 2.0;
    float pulse_factor = (sin(pulse_timer * 4.0) + 1.0) / 2.0;

    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        LevelNode* node = &level_nodes[i];
        bool unlocked = is_level_unlocked(node->level_number);

        ALLEGRO_COLOR fill_color;
        ALLEGRO_COLOR border_color;
        ALLEGRO_COLOR text_color;
        float radius = node->is_special ? BOSS_NODE_RADIUS : NODE_RADIUS;

        if (!unlocked)
        {
            fill_color = al_map_rgb(50, 50, 50);
            border_color = al_map_rgb(30, 30, 30);
            text_color = al_map_rgb(80, 80, 80);

            al_draw_filled_circle(node->x, node->y, radius, fill_color);
            al_draw_circle(node->x, node->y, radius, border_color, 4.0f);
        }
        else if (node->is_special)
        {
            float glow_size = 10.0f + pulse_factor * 8.0f;
            al_draw_filled_circle(node->x, node->y, radius + glow_size,
                                  al_map_rgba(255, 50, 50, (int)(80 * pulse_factor)));
            al_draw_filled_circle(node->x, node->y, radius + glow_size * 0.6f,
                                  al_map_rgba(255, 100, 50, (int)(100 * pulse_factor)));

            int red_base = 180;
            int red_pulse = (int)(75 * blink_factor);
            fill_color = al_map_rgb(red_base + red_pulse, 20, 20);
            border_color = al_map_rgb(255, 200, 50);
            text_color = al_map_rgb(255, 255, 255);

            al_draw_filled_circle(node->x, node->y, radius, fill_color);
            al_draw_circle(node->x, node->y, radius, border_color, 5.0f);
            al_draw_circle(node->x, node->y, radius - 8, al_map_rgba(255, 255, 200, 100), 2.0f);
        }
        else
        {
            fill_color = al_map_rgb(100, 200, 100);
            border_color = al_map_rgb(50, 150, 50);
            text_color = al_map_rgb(255, 255, 255);

            al_draw_filled_circle(node->x, node->y, radius, fill_color);
            al_draw_circle(node->x, node->y, radius, border_color, 4.0f);
        }

        if (node->is_special)
        {
            al_draw_text(level_font, text_color, node->x, node->y - 12, ALLEGRO_ALIGN_CENTRE, "5");
        }
        else
        {
            char level_text[4];
            snprintf(level_text, sizeof(level_text), "%d", node->level_number);
            al_draw_text(level_font, text_color, node->x, node->y - 12, ALLEGRO_ALIGN_CENTRE,
                         level_text);
        }
    }
}

static bool is_point_in_node(float px, float py, LevelNode* node)
{
    float radius = node->is_special ? BOSS_NODE_RADIUS : NODE_RADIUS;
    float dx = px - node->x;
    float dy = py - node->y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

static void update(ALLEGRO_EVENT* event, bool* running)
{
    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        *running = false;
        return;
    }

    if (showing_cutscene)
    {
        if (event->type == ALLEGRO_EVENT_KEY_DOWN || event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            current_cutscene_index++;

            if (current_cutscene_index >= TOTAL_CUTSCENES)
            {
                showing_cutscene = false;
                current_cutscene_index = 0;
                cutscene_already_seen = true;
                in_history_screen = true;
            }
        }
        return;
    }

    if (show_tutorial_modal)
    {
        if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            int mx = event->mouse.x;
            int my = event->mouse.y;

            int modal_w = 600;
            int modal_h = 220;
            int modal_x = (1280 - modal_w) / 2;
            int modal_y = (720 - modal_h) / 2;

            int button_w = 100;
            int button_h = 40;
            int button_x = modal_x + (modal_w - button_w) / 2;
            int button_y = modal_y + modal_h - 60;

            if (mx >= button_x && mx <= button_x + button_w && my >= button_y &&
                my <= button_y + button_h)
            {
                show_tutorial_modal = false;
            }
        }
        return;
    }

    if (in_history_screen)
    {
        if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            in_history_screen = false;
            return;
        }

        if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            int mx = event->mouse.x;
            int my = event->mouse.y;

            int back_x1 = 50;
            int back_y1 = 630;
            int back_x2 = 250;
            int back_y2 = 690;
            if (mx >= back_x1 && mx <= back_x2 && my >= back_y1 && my <= back_y2)
            {
                in_history_screen = false;
                return;
            }

            int replay_x1 = 1030;
            int replay_y1 = 630;
            int replay_x2 = 1230;
            int replay_y2 = 690;
            if (mx >= replay_x1 && mx <= replay_x2 && my >= replay_y1 && my <= replay_y2)
            {
                showing_cutscene = true;
                current_cutscene_index = 0;
                return;
            }

            for (int i = 0; i < TOTAL_LEVELS; i++)
            {
                if (is_point_in_node(mx, my, &level_nodes[i]))
                {
                    if (!is_level_unlocked(level_nodes[i].level_number))
                    {
                        printf("Fase %d bloqueada\n", level_nodes[i].level_number);
                        break;
                    }

                    if (level_nodes[i].is_special)
                    {
                        printf("Fase Especial %d - Boss Final!\n", level_nodes[i].level_number);
                        current_screen->destroy();
                        current_screen = &BossStageScreen;
                        current_screen->init(NULL);
                    }
                    else
                    {
                        set_current_stage(level_nodes[i].level_number);
                        current_screen->destroy();
                        current_screen = &StageScreen;
                        current_screen->init(NULL);
                    }
                    break;
                }
            }
        }
        return;
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        current_screen->destroy();
        current_screen = &MenuScreen;
        current_screen->init(NULL);
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mx = event->mouse.x;
        int my = event->mouse.y;

        int col_x1 = 1050;
        int col_y1 = 170;
        int col_x2 = 1220;
        int col_y2 = 260;
        if (mx >= col_x1 && mx <= col_x2 && my >= col_y1 && my <= col_y2)
        {
            printf("Coleção\n");
            current_screen->destroy();
            current_screen = &Bestiary;
            current_screen->init(NULL);
        }

        int sf_x1 = 100;
        int sf_y1 = 400;
        int sf_x2 = 280;
        int sf_y2 = 510;
        if (mx >= sf_x1 && mx <= sf_x2 && my >= sf_y1 && my <= sf_y2)
        {
            if (Player && Player->current_stage == 1)
            {
                show_tutorial_modal = true;
                return;
            }

            current_screen->destroy();
            current_screen = &EndlessModeScreen;
            current_screen->init(NULL);
        }

        int his_x1 = 360;
        int his_y1 = 130;
        int his_x2 = 950;
        int his_y2 = 460;
        if (mx >= his_x1 && mx <= his_x2 && my >= his_y1 && my <= his_y2)
        {
            if (cutscene_already_seen)
            {
                in_history_screen = true;
            }
            else
            {
                showing_cutscene = true;
                current_cutscene_index = 0;
            }
            printf("História iniciada\n");
        }
    }
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (showing_cutscene)
    {
        if (cutscene_images[current_cutscene_index])
        {
            al_draw_scaled_bitmap(cutscene_images[current_cutscene_index], 0, 0,
                                  al_get_bitmap_width(cutscene_images[current_cutscene_index]),
                                  al_get_bitmap_height(cutscene_images[current_cutscene_index]), 0,
                                  0, screen_width, screen_height, 0);
        }

        al_draw_filled_rectangle(0, screen_height - 200, screen_width, screen_height,
                                 al_map_rgba(0, 0, 0, 200));

        const char* text = cutscenes[current_cutscene_index].text;
        float text_y = screen_height - 180;

        al_draw_multiline_text(cutscene_font, al_map_rgb(255, 255, 255), screen_width / 2, text_y,
                               screen_width - 100, 20, ALLEGRO_ALIGN_CENTRE, text);

        al_draw_text(font, al_map_rgb(200, 200, 200), screen_width - 20, screen_height - 30,
                     ALLEGRO_ALIGN_RIGHT, "Clique ou pressione qualquer tecla");

        return;
    }

    if (show_tutorial_modal)
    {
        if (background)
            al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                                  al_get_bitmap_height(background), 0, 0, 1280, 720, 0);

        ALLEGRO_COLOR blue = al_map_rgb(135, 206, 250);
        ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);

        float circle_x = 1240;
        float circle_y = 50;
        float radius = 25;
        al_draw_filled_circle(circle_x, circle_y, radius, blue);
        al_draw_circle(circle_x, circle_y, radius, black, 3);

        char vaccines_text[10];
        snprintf(vaccines_text, sizeof(vaccines_text), "%d", Player->vaccines);
        al_draw_text(font, black, circle_x, circle_y - 8, ALLEGRO_ALIGN_CENTRE, vaccines_text);

        al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(0, 0, 0, 180));

        int modal_w = 600;
        int modal_h = 220;
        int modal_x = (1280 - modal_w) / 2;
        int modal_y = (720 - modal_h) / 2;

        al_draw_filled_rounded_rectangle(modal_x, modal_y, modal_x + modal_w, modal_y + modal_h, 10,
                                         10, al_map_rgb(40, 40, 60));
        al_draw_rounded_rectangle(modal_x, modal_y, modal_x + modal_w, modal_y + modal_h, 10, 10,
                                  al_map_rgb(150, 150, 150), 3.0f);

        if (font)
        {
            const char* line1 = "MODO SEM FIM BLOQUEADO";
            const char* line2 = "Você precisa completar a primeira fase";
            const char* line3 = "no modo história para desbloquear";
            const char* line4 = "o modo sem fim e concluir o tutorial.";

            al_draw_text(font, al_map_rgb(255, 255, 255), modal_x + modal_w / 2, modal_y + 30,
                         ALLEGRO_ALIGN_CENTER, line1);

            al_draw_text(font, al_map_rgb(200, 200, 200), modal_x + modal_w / 2, modal_y + 70,
                         ALLEGRO_ALIGN_CENTER, line2);

            al_draw_text(font, al_map_rgb(200, 200, 200), modal_x + modal_w / 2, modal_y + 90,
                         ALLEGRO_ALIGN_CENTER, line3);

            al_draw_text(font, al_map_rgb(200, 200, 200), modal_x + modal_w / 2, modal_y + 110,
                         ALLEGRO_ALIGN_CENTER, line4);
        }

        int button_w = 100;
        int button_h = 40;
        int button_x = modal_x + (modal_w - button_w) / 2;
        int button_y = modal_y + modal_h - 60;

        al_draw_filled_rounded_rectangle(button_x, button_y, button_x + button_w,
                                         button_y + button_h, 5, 5, al_map_rgb(0, 180, 0));
        al_draw_rounded_rectangle(button_x, button_y, button_x + button_w, button_y + button_h, 5,
                                  5, al_map_rgb(255, 255, 255), 2.0f);

        if (font)
        {
            al_draw_text(font, al_map_rgb(255, 255, 255), button_x + button_w / 2,
                         button_y + button_h / 2 - 5, ALLEGRO_ALIGN_CENTER, "OK");
        }

        return;
    }

    if (in_history_screen)
    {
        if (background_history)
            al_draw_scaled_bitmap(background_history, 0, 0, al_get_bitmap_width(background_history),
                                  al_get_bitmap_height(background_history), 0, 0, 1280, 720, 0);

        al_draw_text(level_font, al_map_rgb(255, 255, 255), 640, 90, ALLEGRO_ALIGN_CENTRE,
                     "SELECIONE UMA FASE");

        draw_level_connections();
        draw_level_nodes();

        al_draw_filled_rounded_rectangle(50, 630, 250, 690, 8, 8, al_map_rgb(80, 80, 80));
        al_draw_rounded_rectangle(50, 630, 250, 690, 8, 8, al_map_rgb(255, 255, 255), 2);
        al_draw_text(font, al_map_rgb(255, 255, 255), 150, 652, ALLEGRO_ALIGN_CENTRE, "Voltar");

        al_draw_filled_rounded_rectangle(1030, 630, 1230, 690, 8, 8, al_map_rgb(70, 130, 180));
        al_draw_rounded_rectangle(1030, 630, 1230, 690, 8, 8, al_map_rgb(255, 255, 255), 2);
        al_draw_text(font, al_map_rgb(255, 255, 255), 1130, 652, ALLEGRO_ALIGN_CENTRE,
                     "Ver Cutscene");

        return;
    }

    if (background)
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, 1280, 720, 0);

    draw_vaccines_count(screen_width, screen_height);
}

static void destroy(void)
{
    if (background)
    {
        al_destroy_bitmap(background);
        background = NULL;
    }
    if (background_history)
    {
        al_destroy_bitmap(background_history);
        background_history = NULL;
    }
    if (font)
    {
        al_destroy_font(font);
        font = NULL;
    }
    if (cutscene_font)
    {
        al_destroy_font(cutscene_font);
        cutscene_font = NULL;
    }
    if (level_font)
    {
        al_destroy_font(level_font);
        level_font = NULL;
    }
    if (boss_font)
    {
        al_destroy_font(boss_font);
        boss_font = NULL;
    }

    for (int i = 0; i < TOTAL_CUTSCENES; i++)
    {
        if (cutscene_images[i])
        {
            al_destroy_bitmap(cutscene_images[i]);
            cutscene_images[i] = NULL;
        }
    }
}

Screen LobbyScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
