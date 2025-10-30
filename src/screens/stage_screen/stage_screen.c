#include "./stage_screen.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../core/game.h"
#include "../../entities/player/player-entity.h"
#include "../base/menu.h"
#include "../lobby_screen/lobby_screen.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* background = NULL;
static ALLEGRO_BITMAP* defender_images[MAX_IN_USE_SLOTS] = {NULL, NULL, NULL};
static int defender_w[MAX_IN_USE_SLOTS] = {0, 0, 0};
static int defender_h[MAX_IN_USE_SLOTS] = {0, 0, 0};
static ALLEGRO_BITMAP* enemy_images[3] = {NULL, NULL, NULL};
static int enemy_w[3] = {0, 0, 0};
static int enemy_h[3] = {0, 0, 0};

#define GRID_COLS 9
#define GRID_ROWS 5
#define GRID_START_Y 90
#define SELECTOR_HEIGHT 60

#define MAX_DEFENDERS 45
#define MAX_ENEMIES 50
#define MAX_PROJECTILES 100
#define MAX_PARTICLES 200
#define MAX_ORBS 60

typedef enum
{
    ENEMY_VIRUS = 0,
    ENEMY_PARASITE = 1,
    ENEMY_BACTERIUM = 2
} EnemyType;

typedef enum
{
    PROJECTILE_WHITE_BALL = 0,
    PROJECTILE_BLUE_MAGIC = 1,
    PROJECTILE_RED_LASER = 2
} ProjectileType;

typedef struct
{
    float x;
    float y;
    float vx;
    float vy;
    float life;
    float max_life;
    ALLEGRO_COLOR color;
} Particle;

typedef struct
{
    float x;
    float y;
    float vy;
    bool active;
    float rotation;
} Orb;

typedef struct
{
    int row;
    int col;
    int defender_slot;
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
    ProjectileType type;
    float animation_time;
    int damage;
} Projectile;

typedef struct
{
    int total_waves;
    int enemies_per_wave;
    float wave_interval;
    int stage_number;
} StageConfig;

static Particle particles[MAX_PARTICLES];
static Orb orbs[MAX_ORBS];
static Defender defenders[MAX_DEFENDERS];
static Enemy enemies[MAX_ENEMIES];
static Projectile projectiles[MAX_PROJECTILES];

static int selected_defender_slot = -1;
static int in_use_defenders[MAX_IN_USE_SLOTS];
static int in_use_count = 0;

static float placement_cooldown = 0.0f;
static float delta_time = 1.0f / 60.0f;
static float game_time = 0.0f;
static int vitamins = 100;
static bool stage_complete = false;
static bool stage_failed = false;

static int current_wave = 0;
static int enemies_killed_this_stage = 0;
static float wave_timer = 0.0f;
static bool wave_active = false;
static int enemies_spawned_in_wave = 0;

static float cell_width = 0.0f;
static float cell_height = 0.0f;
static int screen_width_cached = 0;
static int screen_height_cached = 0;
static ALLEGRO_FONT* font = NULL;
static ALLEGRO_FONT* title_font = NULL;

static const int defender_costs[3] = {50, 75, 100};
static const int defender_damage[3] = {1, 2, 3};
static const char* defender_paths[10] = {
    "assets/images/defenders/white_blood_cell.png", "assets/images/defenders/eosinophil.png",
    "assets/images/defenders/basophil.png",         "assets/images/defenders/defender_3.png",
    "assets/images/defenders/defender_4.png",       "assets/images/defenders/defender_5.png",
    "assets/images/defenders/defender_6.png",       "assets/images/defenders/defender_7.png",
    "assets/images/defenders/defender_8.png",       "assets/images/defenders/defender_9.png"};

static StageConfig stage_config;
static double last_time = 0.0;

static void init_arrays(void)
{
    for (int i = 0; i < MAX_DEFENDERS; i++) defenders[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (int i = 0; i < MAX_PROJECTILES; i++) projectiles[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0.0f;
    for (int i = 0; i < MAX_ORBS; i++) orbs[i].active = false;
}

static void load_in_use_defenders(void)
{
    in_use_count = 0;
    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        int defender_id = PLAYER_ENTITY->in_use_slots[i];
        if (defender_id >= 0 && PLAYER_ENTITY->defenders[defender_id].unlocked)
        {
            in_use_defenders[in_use_count] = defender_id;
            in_use_count++;
        }
    }
}

static void configure_stage(int stage_number)
{
    stage_config.stage_number = stage_number;
    stage_config.total_waves = 3 + (stage_number / 2);
    stage_config.enemies_per_wave = 5 + stage_number;
    stage_config.wave_interval = 15.0f;
}

void set_current_stage(int stage_number) { configure_stage(stage_number); }

static void spawn_particle_burst(float x, float y, int count, ALLEGRO_COLOR color)
{
    int spawned = 0;
    for (int j = 0; j < MAX_PARTICLES && spawned < count; j++)
    {
        if (particles[j].life <= 0.0f)
        {
            particles[j].x = x;
            particles[j].y = y;

            float angle = (2.0f * 3.14159f * spawned) / (float)count;
            float speed = 100.0f + (rand() % 100);

            particles[j].vx = cos(angle) * speed;
            particles[j].vy = sin(angle) * speed;
            particles[j].max_life = 0.5f;
            particles[j].life = 0.5f;
            particles[j].color = color;
            spawned++;
        }
    }
}

static void spawn_orb(void)
{
    for (int i = 0; i < MAX_ORBS; i++)
    {
        if (!orbs[i].active)
        {
            orbs[i].active = true;
            orbs[i].x =
                50.0f + (rand() % (screen_width_cached > 100 ? screen_width_cached - 100 : 1));
            orbs[i].y = -30.0f;
            orbs[i].vy = 80.0f + (rand() % 40);
            orbs[i].rotation = 0.0f;
            break;
        }
    }
}

static void load_images(void)
{
    for (int i = 0; i < in_use_count; i++)
    {
        int defender_id = in_use_defenders[i];
        if (defender_id >= 0 && defender_id < 10)
        {
            defender_images[i] = al_load_bitmap(defender_paths[defender_id]);
            if (defender_images[i])
            {
                defender_w[i] = al_get_bitmap_width(defender_images[i]);
                defender_h[i] = al_get_bitmap_height(defender_images[i]);
            }
            else
            {
                defender_w[i] = defender_h[i] = 0;
            }
        }
    }

    enemy_images[0] = al_load_bitmap("assets/images/enemies/virus.png");
    enemy_images[1] = al_load_bitmap("assets/images/enemies/parasite.png");
    enemy_images[2] = al_load_bitmap("assets/images/enemies/bacterium.png");

    for (int i = 0; i < 3; i++)
    {
        if (enemy_images[i])
        {
            enemy_w[i] = al_get_bitmap_width(enemy_images[i]);
            enemy_h[i] = al_get_bitmap_height(enemy_images[i]);
        }
        else
        {
            enemy_w[i] = enemy_h[i] = 0;
        }
    }
}

static void spawn_wave_enemy(int screen_width)
{
    if (enemies_spawned_in_wave >= stage_config.enemies_per_wave)
        return;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].active = true;
            enemies[i].row = rand() % GRID_ROWS;
            enemies[i].type = rand() % 3;
            enemies[i].x = (float)screen_width;
            enemies[i].y = GRID_START_Y + enemies[i].row * cell_height + cell_height / 2.0f;
            float base_speed = 35.0f + (stage_config.stage_number * 3.0f);
            enemies[i].speed = base_speed;
            enemies[i].health = 4 + (stage_config.stage_number / 2);
            enemies_spawned_in_wave++;
            break;
        }
    }
}

static void add_defender(int row, int col, int defender_slot)
{
    if (placement_cooldown > 0.0f)
        return;

    int cost = defender_costs[defender_slot];
    if (vitamins < cost)
        return;

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].active && defenders[i].row == row && defenders[i].col == col)
            return;
    }

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (!defenders[i].active)
        {
            defenders[i].active = true;
            defenders[i].row = row;
            defenders[i].col = col;
            defenders[i].defender_slot = defender_slot;
            defenders[i].shoot_timer = 0.0f;
            placement_cooldown = 1.0f;
            vitamins -= cost;
            selected_defender_slot = -1;
            break;
        }
    }
}

static void shoot_projectile(int row, int col, int defender_slot)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!projectiles[i].active)
        {
            projectiles[i].active = true;
            projectiles[i].row = row;
            projectiles[i].x = col * (cell_width + 1.0f) + cell_width;
            projectiles[i].y = GRID_START_Y + row * cell_height + cell_height / 2.0f;
            projectiles[i].animation_time = 0.0f;
            projectiles[i].damage = defender_damage[defender_slot];

            if (defender_slot == 0)
            {
                projectiles[i].type = PROJECTILE_WHITE_BALL;
                projectiles[i].speed = 180.0f;
            }
            else if (defender_slot == 1)
            {
                projectiles[i].type = PROJECTILE_BLUE_MAGIC;
                projectiles[i].speed = 220.0f;
            }
            else
            {
                projectiles[i].type = PROJECTILE_RED_LASER;
                projectiles[i].speed = 300.0f;
            }
            break;
        }
    }
}

static void check_enemy_defender_collision(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            for (int j = 0; j < MAX_DEFENDERS; j++)
            {
                if (defenders[j].active && defenders[j].row == enemies[i].row)
                {
                    float defender_x = defenders[j].col * (cell_width + 1.0f) + cell_width / 2.0f;
                    float defender_y =
                        GRID_START_Y + defenders[j].row * cell_height + cell_height / 2.0f;
                    float dx = enemies[i].x - defender_x;
                    float dy = enemies[i].y - defender_y;
                    float dist2 = dx * dx + dy * dy;

                    if (dist2 < 40.0f * 40.0f)
                    {
                        spawn_particle_burst(enemies[i].x, enemies[i].y, 20,
                                             al_map_rgb(255, 100, 100));
                        spawn_particle_burst(defender_x, defender_y, 20, al_map_rgb(200, 50, 50));
                        defenders[j].active = false;
                        enemies[i].active = false;
                        break;
                    }
                }
            }
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
                    shoot_projectile(defenders[i].row, defenders[i].col,
                                     defenders[i].defender_slot);
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

            if (enemies[i].x < -50.0f)
            {
                enemies[i].active = false;
                stage_failed = true;
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
            projectiles[i].animation_time += delta_time;

            if (projectiles[i].x > (float)screen_width)
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
                        enemies[j].health -= projectiles[i].damage;
                        if (enemies[j].health <= 0)
                        {
                            spawn_particle_burst(enemies[j].x, enemies[j].y, 25,
                                                 al_map_rgb(255, 200, 50));
                            enemies[j].active = false;
                            enemies_killed_this_stage++;
                        }
                        projectiles[i].active = false;
                        break;
                    }
                }
            }
        }
    }
}

static void update_particles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life > 0.0f)
        {
            particles[i].x += particles[i].vx * delta_time;
            particles[i].y += particles[i].vy * delta_time;
            particles[i].vy += 200.0f * delta_time;
            particles[i].life -= delta_time;
        }
    }
}

static void update_orbs(void)
{
    for (int i = 0; i < MAX_ORBS; i++)
    {
        if (orbs[i].active)
        {
            orbs[i].y += orbs[i].vy * delta_time;
            orbs[i].rotation += 5.0f;
            if (orbs[i].y > (float)screen_height_cached + 50.0f)
                orbs[i].active = false;
        }
    }
}

static bool check_all_enemies_defeated(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
            return false;
    }
    return true;
}

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_PLAYING;
    srand((unsigned int)time(NULL));
    init_arrays();
    load_in_use_defenders();

    game_time = 0.0f;
    vitamins = 150;
    stage_complete = false;
    stage_failed = false;
    current_wave = 0;
    enemies_killed_this_stage = 0;
    wave_timer = 0.0f;
    wave_active = false;
    enemies_spawned_in_wave = 0;

    background = al_load_bitmap("assets/images/maps/initial.png");
    load_images();
    font = al_create_builtin_font();
    title_font = al_create_builtin_font();

    if (display)
    {
        screen_width_cached = al_get_display_width(display);
        screen_height_cached = al_get_display_height(display);
        cell_width = ((float)screen_width_cached / GRID_COLS) - 5.0f;
        cell_height = (float)(screen_height_cached - GRID_START_Y) / GRID_ROWS;
    }

    last_time = al_get_time();
}

static void update(ALLEGRO_EVENT* event, bool* running)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        current_screen->destroy();
        current_screen = &LobbyScreen;
        current_screen->init(NULL);
    }

    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        *running = false;
    }

    if (stage_complete || stage_failed)
    {
        if (event->type == ALLEGRO_EVENT_KEY_DOWN || event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (stage_complete && PLAYER_ENTITY->current_stage == stage_config.stage_number)
            {
                PLAYER_ENTITY->current_stage++;
            }

            current_screen->destroy();
            current_screen = &LobbyScreen;
            current_screen->init(NULL);
        }
        return;
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (cell_width <= 0.0f || cell_height <= 0.0f || screen_width_cached <= 0)
            return;
        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        for (int i = 0; i < MAX_ORBS; i++)
        {
            if (orbs[i].active)
            {
                float dx = mouse_x - orbs[i].x;
                float dy = mouse_y - orbs[i].y;
                float dist2 = dx * dx + dy * dy;
                if (dist2 < 20.0f * 20.0f)
                {
                    vitamins += 75;
                    orbs[i].active = false;
                    spawn_particle_burst(orbs[i].x, orbs[i].y, 15, al_map_rgb(255, 215, 0));
                    return;
                }
            }
        }

        if (mouse_y < SELECTOR_HEIGHT)
        {
            int selector_width = 100;
            int start_x = 10;

            for (int i = 0; i < in_use_count; i++)
            {
                int x1 = start_x + i * (selector_width + 10);
                int x2 = x1 + selector_width;
                if (mouse_x >= x1 && mouse_x <= x2 && mouse_y >= 10 &&
                    mouse_y <= SELECTOR_HEIGHT - 10)
                {
                    if (vitamins >= defender_costs[i])
                        selected_defender_slot = i;
                    else
                        selected_defender_slot = -1;
                    break;
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
                if (selected_defender_slot != -1)
                    add_defender(row, col, selected_defender_slot);
            }
        }
    }

    if (event->type == ALLEGRO_EVENT_TIMER)
    {
        if (screen_width_cached <= 0)
            return;

        double now = al_get_time();
        double dt = now - last_time;
        last_time = now;

        if (dt <= 0.0)
            dt = 1.0 / 60.0;
        if (dt > 0.1)
            dt = 0.1;

        delta_time = (float)dt;
        game_time += delta_time;

        if (placement_cooldown > 0.0f)
            placement_cooldown -= delta_time;

        update_defenders();
        update_enemies();
        update_projectiles(screen_width_cached);
        update_particles();
        update_orbs();
        check_enemy_defender_collision();

        if (!wave_active)
        {
            wave_timer += delta_time;
            if (wave_timer >= 3.0f && current_wave < stage_config.total_waves)
            {
                wave_active = true;
                wave_timer = 0.0f;
                enemies_spawned_in_wave = 0;
                current_wave++;
            }
        }
        else
        {
            wave_timer += delta_time;
            if (wave_timer >= 1.5f && enemies_spawned_in_wave < stage_config.enemies_per_wave)
            {
                spawn_wave_enemy(screen_width_cached);
                wave_timer = 0.0f;
            }

            if (enemies_spawned_in_wave >= stage_config.enemies_per_wave &&
                check_all_enemies_defeated())
            {
                wave_active = false;
                wave_timer = 0.0f;
            }
        }

        if (current_wave >= stage_config.total_waves && !wave_active &&
            check_all_enemies_defeated())
        {
            stage_complete = true;
        }

        if (stage_failed)
        {
            return;
        }

        if (rand() % 1000 < 3)
            spawn_orb();
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

        int selector_width = 100;
        int start_x = 10;

        for (int i = 0; i < in_use_count; i++)
        {
            int x1 = start_x + i * (selector_width + 10);
            int x2 = x1 + selector_width;
            ALLEGRO_COLOR color;
            ALLEGRO_COLOR border_color;
            ALLEGRO_COLOR text_color;

            if (vitamins >= defender_costs[i])
            {
                color = (selected_defender_slot == i) ? al_map_rgba(0, 255, 0, 180)
                                                      : al_map_rgba(80, 80, 80, 150);
                border_color = al_map_rgb(255, 255, 255);
                text_color = al_map_rgb(255, 215, 0);
            }
            else
            {
                color = al_map_rgba(40, 40, 40, 80);
                border_color = al_map_rgb(100, 100, 100);
                text_color = al_map_rgb(150, 150, 150);
            }

            al_draw_filled_rectangle(x1, 10, x2, SELECTOR_HEIGHT - 10, color);
            al_draw_rectangle(x1, 10, x2, SELECTOR_HEIGHT - 10, border_color, 2.0f);

            if (defender_images[i])
            {
                al_draw_scaled_bitmap(defender_images[i], 0, 0, defender_w[i], defender_h[i],
                                      x1 + 35, 15, 35, 35, 0);
            }

            char cost_text[16];
            sprintf(cost_text, "C:%d", defender_costs[i]);
            if (font)
                al_draw_text(font, text_color, x1 + 50, 50, ALLEGRO_ALIGN_CENTER, cost_text);
        }

        char stage_text[32];
        sprintf(stage_text, "Fase %d", stage_config.stage_number);
        al_draw_filled_rectangle(screen_width / 2 - 100, 10, screen_width / 2 + 100, 50,
                                 al_map_rgba(0, 0, 0, 180));
        if (font)
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, 25,
                         ALLEGRO_ALIGN_CENTER, stage_text);

        char wave_text[32];
        sprintf(wave_text, "Onda: %d/%d", current_wave, stage_config.total_waves);
        al_draw_filled_rectangle(screen_width - 150, 10, screen_width - 10, 50,
                                 al_map_rgba(0, 0, 0, 180));
        if (font)
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width - 80, 25,
                         ALLEGRO_ALIGN_CENTER, wave_text);

        char vitamins_text[32];
        sprintf(vitamins_text, "Vitaminas: %d", vitamins);
        al_draw_filled_rectangle(screen_width - 150, 60, screen_width - 10, 100,
                                 al_map_rgba(0, 0, 0, 180));
        if (font)
            al_draw_text(font, al_map_rgb(255, 215, 0), screen_width - 80, 78, ALLEGRO_ALIGN_CENTER,
                         vitamins_text);

        for (int i = 0; i < MAX_DEFENDERS; i++)
        {
            if (defenders[i].active)
            {
                int slot = defenders[i].defender_slot;
                if (slot >= 0 && slot < in_use_count && defender_images[slot])
                {
                    float x = defenders[i].col * (cell_width + 1.0f);
                    float y = GRID_START_Y + defenders[i].row * cell_height;
                    float scale = (defender_w[slot] > 0)
                                      ? (cell_width * 0.8f) / (float)defender_w[slot]
                                      : 1.0f;
                    float img_w = defender_w[slot] * scale;
                    float img_h = defender_h[slot] * scale;

                    al_draw_scaled_bitmap(defender_images[slot], 0, 0, defender_w[slot],
                                          defender_h[slot], x + (cell_width - img_w) / 2.0f,
                                          y + (cell_height - 10.0f - img_h) / 2.0f, img_w, img_h,
                                          0);
                }
            }
        }

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active && enemy_images[enemies[i].type])
            {
                float size = 60.0f;
                al_draw_scaled_bitmap(enemy_images[enemies[i].type], 0, 0, enemy_w[enemies[i].type],
                                      enemy_h[enemies[i].type], enemies[i].x - size / 2.0f,
                                      enemies[i].y - size / 2.0f, size, size, 0);
            }
        }

        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (projectiles[i].active)
            {
                float anim = projectiles[i].animation_time;

                if (projectiles[i].type == PROJECTILE_WHITE_BALL)
                {
                    float pulse = sin(anim * 10.0f) * 2.0f;
                    al_draw_filled_circle(projectiles[i].x, projectiles[i].y, 12.0f + pulse,
                                          al_map_rgb(255, 255, 255));
                }
                else if (projectiles[i].type == PROJECTILE_BLUE_MAGIC)
                {
                    al_draw_filled_circle(projectiles[i].x, projectiles[i].y, 8.0f,
                                          al_map_rgb(100, 200, 255));
                }
                else if (projectiles[i].type == PROJECTILE_RED_LASER)
                {
                    al_draw_filled_rectangle(projectiles[i].x - 20, projectiles[i].y - 3,
                                             projectiles[i].x + 20, projectiles[i].y + 3,
                                             al_map_rgb(255, 30, 30));
                }
            }
        }

        for (int i = 0; i < MAX_ORBS; i++)
        {
            if (orbs[i].active)
            {
                al_draw_filled_circle(orbs[i].x, orbs[i].y, 15.0f, al_map_rgb(255, 215, 0));
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (particles[i].life > 0.0f)
            {
                float alpha_factor = particles[i].life / particles[i].max_life;
                ALLEGRO_COLOR particle_color = particles[i].color;
                particle_color.a = alpha_factor;
                al_draw_filled_circle(particles[i].x, particles[i].y, 3.0f, particle_color);
            }
        }
    }

    if (stage_complete)
    {
        al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 200));
        if (title_font)
        {
            al_draw_text(title_font, al_map_rgb(0, 255, 0), screen_width / 2,
                         screen_height / 2 - 50, ALLEGRO_ALIGN_CENTER, "FASE COMPLETA!");
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2 + 20,
                         ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para continuar");
        }
    }

    if (stage_failed)
    {
        al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 200));
        if (title_font)
        {
            al_draw_text(title_font, al_map_rgb(255, 0, 0), screen_width / 2,
                         screen_height / 2 - 50, ALLEGRO_ALIGN_CENTER, "FASE FALHOU!");
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2 + 20,
                         ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para tentar novamente");
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

    if (font)
    {
        al_destroy_font(font);
        font = NULL;
    }

    if (title_font)
    {
        al_destroy_font(title_font);
        title_font = NULL;
    }

    for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
    {
        if (defender_images[i])
        {
            al_destroy_bitmap(defender_images[i]);
            defender_images[i] = NULL;
            defender_w[i] = defender_h[i] = 0;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (enemy_images[i])
        {
            al_destroy_bitmap(enemy_images[i]);
            enemy_images[i] = NULL;
            enemy_w[i] = enemy_h[i] = 0;
        }
    }
}

Screen StageScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
