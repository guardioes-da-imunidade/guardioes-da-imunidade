#include "./endless_mode_screen.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../base/menu.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* background = NULL;

static ALLEGRO_BITMAP* defender_images[3] = {NULL, NULL, NULL};
static ALLEGRO_BITMAP* enemy_images[3] = {NULL, NULL, NULL};

#define GRID_COLS 9
#define GRID_ROWS 5
#define GRID_START_Y 120
#define SELECTOR_HEIGHT 100

#define MAX_DEFENDERS 45
#define MAX_ENEMIES 50
#define MAX_PROJECTILES 100

typedef enum
{
    DEFENDER_WHITE_BLOOD_CELL = 0,
    DEFENDER_EOSINOPHIL = 1,
    DEFENDER_BASOPHIL = 2
} DefenderType;

typedef enum
{
    ENEMY_VIRUS = 0,
    ENEMY_PARASITE = 1,
    ENEMY_BACTERIUM = 2
} EnemyType;

typedef struct
{
    int row;
    int col;
    DefenderType type;
    float shoot_timer;
    bool active;
} Defender;

typedef struct
{
    float x;
    float y;
    int row;
    EnemyType type;
    float speed;
    bool active;
    int health;
} Enemy;

typedef struct
{
    float x;
    float y;
    int row;
    float speed;
    bool active;
} Projectile;

static Defender defenders[MAX_DEFENDERS];
static Enemy enemies[MAX_ENEMIES];
static Projectile projectiles[MAX_PROJECTILES];

static DefenderType selected_defender = DEFENDER_WHITE_BLOOD_CELL;
static float enemy_spawn_timer = 0.0f;
static float delta_time = 1.0f / 60.0f;

static float cell_width = 0.0f;
static float cell_height = 0.0f;
static int screen_width_cached = 0;
static int screen_height_cached = 0;

static void init_arrays(void)
{
    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        defenders[i].active = false;
    }
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        projectiles[i].active = false;
    }
}

static void load_images(void)
{
    defender_images[0] = al_load_bitmap("assets/images/defenders/white_blood_cell.png");
    defender_images[1] = al_load_bitmap("assets/images/defenders/eosinophil.png");
    defender_images[2] = al_load_bitmap("assets/images/defenders/basophil.png");

    enemy_images[0] = al_load_bitmap("assets/images/enemies/virus.png");
    enemy_images[1] = al_load_bitmap("assets/images/enemies/parasite.png");
    enemy_images[2] = al_load_bitmap("assets/images/enemies/bacterium.png");

    for (int i = 0; i < 3; i++)
    {
        if (!defender_images[i])
        {
            fprintf(stderr, "Erro ao carregar imagem do defensor %d!\n", i);
        }
        if (!enemy_images[i])
        {
            fprintf(stderr, "Erro ao carregar imagem do inimigo %d!\n", i);
        }
    }
}

static void spawn_enemy(int screen_width)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].active = true;
            enemies[i].row = rand() % GRID_ROWS;
            enemies[i].type = rand() % 3;
            enemies[i].x = screen_width;
            enemies[i].y = GRID_START_Y + enemies[i].row * cell_height + cell_height / 2;
            enemies[i].speed = 30.0f + (rand() % 20);
            enemies[i].health = 3;
            break;
        }
    }
}

static void add_defender(int row, int col, DefenderType type)
{
    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].active && defenders[i].row == row && defenders[i].col == col)
        {
            return;
        }
    }

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (!defenders[i].active)
        {
            defenders[i].active = true;
            defenders[i].row = row;
            defenders[i].col = col;
            defenders[i].type = type;
            defenders[i].shoot_timer = 0.0f;
            printf("Defensor colocado em [%d][%d]\n", row, col);
            break;
        }
    }
}

static void shoot_projectile(int row, int col)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!projectiles[i].active)
        {
            projectiles[i].active = true;
            projectiles[i].row = row;
            projectiles[i].x = col * (cell_width + 1.0f) + cell_width;
            projectiles[i].y = GRID_START_Y + row * cell_height + cell_height / 2;
            projectiles[i].speed = 200.0f;
            break;
        }
    }
}

static void update_defenders(void)
{
    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].active)
        {
            defenders[i].shoot_timer += delta_time;

            if (defenders[i].shoot_timer >= 3.0f)
            {
                bool enemy_in_row = false;
                for (int j = 0; j < MAX_ENEMIES; j++)
                {
                    if (enemies[j].active && enemies[j].row == defenders[i].row)
                    {
                        enemy_in_row = true;
                        break;
                    }
                }

                if (enemy_in_row)
                {
                    shoot_projectile(defenders[i].row, defenders[i].col);
                }

                defenders[i].shoot_timer = 0.0f;
            }
        }
    }
}

static void update_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            enemies[i].x -= enemies[i].speed * delta_time;

            if (enemies[i].x < -50)
            {
                enemies[i].active = false;
            }
        }
    }
}

static void update_projectiles(int screen_width)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i].active)
        {
            projectiles[i].x += projectiles[i].speed * delta_time;

            if (projectiles[i].x > screen_width)
            {
                projectiles[i].active = false;
                continue;
            }

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (enemies[j].active && enemies[j].row == projectiles[i].row)
                {
                    float dist = fabs(projectiles[i].x - enemies[j].x);
                    if (dist < 30.0f)
                    {
                        enemies[j].health--;
                        if (enemies[j].health <= 0)
                        {
                            enemies[j].active = false;
                        }
                        projectiles[i].active = false;
                        break;
                    }
                }
            }
        }
    }
}

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_PLAYING;

    srand(time(NULL));
    init_arrays();

    background = al_load_bitmap("assets/images/maps/initial.png");
    if (!background)
    {
        fprintf(stderr, "Erro ao carregar imagem de fundo!\n");
    }

    load_images();

    if (display)
    {
        screen_width_cached = al_get_display_width(display);
        screen_height_cached = al_get_display_height(display);

        cell_width = ((float)screen_width_cached / GRID_COLS) - 5.0f;
        cell_height = (float)(screen_height_cached - GRID_START_Y) / GRID_ROWS;
    }
}

static void update(ALLEGRO_EVENT* event, bool* running)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        current_screen->destroy();
        current_screen = &MenuScreen;
        current_screen->init(NULL);
    }

    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        *running = false;
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (cell_width <= 0 || cell_height <= 0 || screen_width_cached <= 0)
        {
            return;
        }

        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        if (mouse_y < SELECTOR_HEIGHT)
        {
            int selector_width = screen_width_cached / 3;
            if (selector_width > 0)
            {
                int clicked_defender = mouse_x / selector_width;
                if (clicked_defender >= 0 && clicked_defender < 3)
                {
                    selected_defender = (DefenderType)clicked_defender;
                    printf("Defensor selecionado: %d\n", selected_defender);
                }
            }
            return;
        }

        if (mouse_y >= GRID_START_Y)
        {
            int col = (int)(mouse_x / (cell_width + 1.0f));
            int row = (int)((mouse_y - GRID_START_Y) / cell_height);

            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS)
            {
                add_defender(row, col, selected_defender);
            }
        }
    }

    if (event->type == ALLEGRO_EVENT_TIMER)
    {
        if (screen_width_cached <= 0)
        {
            return;
        }

        update_defenders();
        update_enemies();
        update_projectiles(screen_width_cached);

        enemy_spawn_timer += delta_time;
        if (enemy_spawn_timer >= 2.0f)
        {
            spawn_enemy(screen_width_cached);
            enemy_spawn_timer = 0.0f;
        }
    }
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (screen_width_cached != screen_width || screen_height_cached != screen_height)
    {
        screen_width_cached = screen_width;
        screen_height_cached = screen_height;
        cell_width = ((float)screen_width / GRID_COLS) - 5.0f;
        cell_height = (float)(screen_height - GRID_START_Y) / GRID_ROWS;
    }

    if (background)
    {
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);

        int selector_width = screen_width / 3;
        for (int i = 0; i < 3; i++)
        {
            ALLEGRO_COLOR color = (selected_defender == i) ? al_map_rgba(0, 255, 0, 150)
                                                           : al_map_rgba(100, 100, 100, 150);

            al_draw_filled_rectangle(i * selector_width + 5, 10, (i + 1) * selector_width - 5,
                                     SELECTOR_HEIGHT - 10, color);

            al_draw_rectangle(i * selector_width + 5, 10, (i + 1) * selector_width - 5,
                              SELECTOR_HEIGHT - 10, al_map_rgb(255, 255, 255), 3.0f);

            if (defender_images[i])
            {
                float scale = 60.0f / al_get_bitmap_width(defender_images[i]);
                al_draw_scaled_bitmap(defender_images[i], 0, 0,
                                      al_get_bitmap_width(defender_images[i]),
                                      al_get_bitmap_height(defender_images[i]),
                                      i * selector_width + selector_width / 2 - 30, 25, 60, 60, 0);
            }
        }

        for (int row = 0; row < GRID_ROWS; row++)
        {
            for (int col = 0; col < GRID_COLS; col++)
            {
                float x = col * (cell_width + 1.0f);
                float y = GRID_START_Y + row * cell_height;

                al_draw_rectangle(x, y, x + cell_width, (y + cell_height) - 10,
                                  al_map_rgba(255, 0, 0, 150), 2.0f);
            }
        }

        for (int i = 0; i < MAX_DEFENDERS; i++)
        {
            if (defenders[i].active && defender_images[defenders[i].type])
            {
                float x = defenders[i].col * (cell_width + 1.0f);
                float y = GRID_START_Y + defenders[i].row * cell_height;

                float scale =
                    (cell_width * 0.8f) / al_get_bitmap_width(defender_images[defenders[i].type]);
                float img_w = al_get_bitmap_width(defender_images[defenders[i].type]) * scale;
                float img_h = al_get_bitmap_height(defender_images[defenders[i].type]) * scale;

                al_draw_scaled_bitmap(defender_images[defenders[i].type], 0, 0,
                                      al_get_bitmap_width(defender_images[defenders[i].type]),
                                      al_get_bitmap_height(defender_images[defenders[i].type]),
                                      x + (cell_width - img_w) / 2,
                                      y + (cell_height - 10 - img_h) / 2, img_w, img_h, 0);
            }
        }

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active && enemy_images[enemies[i].type])
            {
                float size = 60.0f;
                al_draw_scaled_bitmap(enemy_images[enemies[i].type], 0, 0,
                                      al_get_bitmap_width(enemy_images[enemies[i].type]),
                                      al_get_bitmap_height(enemy_images[enemies[i].type]),
                                      enemies[i].x - size / 2, enemies[i].y - size / 2, size, size,
                                      0);
            }
        }

        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (projectiles[i].active)
            {
                al_draw_filled_circle(projectiles[i].x, projectiles[i].y, 5.0f,
                                      al_map_rgb(255, 255, 0));
            }
        }
    }
}

static void destroy(void)
{
    if (background)
    {
        al_destroy_bitmap(background);
        background = NULL;
    }

    for (int i = 0; i < 3; i++)
    {
        if (defender_images[i])
        {
            al_destroy_bitmap(defender_images[i]);
            defender_images[i] = NULL;
        }
        if (enemy_images[i])
        {
            al_destroy_bitmap(enemy_images[i]);
            enemy_images[i] = NULL;
        }
    }
}

Screen EndlessModeScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
