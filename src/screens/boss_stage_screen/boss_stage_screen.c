#include "./boss_stage_screen.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../core/game.h"
#include "../../entities/player/player.h"
#include "../base/menu.h"
#include "../lobby_screen/lobby_screen.h"

extern GameState current_game_state;
extern Screen* current_screen;

static ALLEGRO_BITMAP* boss_image = NULL;
static int boss_w = 0;
static int boss_h = 0;

#define GRID_COLS 9
#define GRID_ROWS 5
#define GRID_START_Y 90
#define SELECTOR_HEIGHT 60

#define MAX_DEFENDERS 45
#define MAX_PROJECTILES 100
#define MAX_PARTICLES 200
#define MAX_ORBS 60
#define MAX_BOSS_PROJECTILES 30

typedef enum
{
    BOSS_INACTIVE = 0,
    BOSS_COUNTDOWN,
    BOSS_ENTERING,
    BOSS_ACTIVE,
    BOSS_DEFEATED
} BossState;

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
    float x;
    float y;
    float vx;
    float vy;
    bool active;
    int target_col;
    int target_row;
} BossProjectile;

typedef struct
{
    BossState state;
    float x;
    float y;
    int max_health;
    int health;
    float attack_timer;
    float attack_cooldown;
    int laser_row;
    float laser_timer;
    float laser_duration;
    bool laser_active;
    int countdown;
    float countdown_timer;
    float enter_progress;
} Boss;

typedef struct
{
    float hue;
    float pulse;
    float shake_x;
    float shake_y;
} GridCell;

static Particle particles[MAX_PARTICLES];
static Orb orbs[MAX_ORBS];
static Defender defenders[MAX_DEFENDERS];
static Projectile projectiles[MAX_PROJECTILES];
static BossProjectile boss_projectiles[MAX_BOSS_PROJECTILES];
static Boss boss;
static GridCell grid_cells[GRID_ROWS][GRID_COLS];

static int selected_defender = -1;

static float placement_cooldown = 0.0f;
static float delta_time = 1.0f / 60.0f;
static float game_time = 0.0f;
static int vitamins = 100;
static bool stage_complete = false;
static bool stage_failed = false;

static float cell_width = 0.0f;
static float cell_height = 0.0f;
static int screen_width_cached = 0;
static int screen_height_cached = 0;
static ALLEGRO_FONT* font = NULL;
static ALLEGRO_FONT* title_font = NULL;
static ALLEGRO_FONT* countdown_font = NULL;

static double last_time = 0.0;
static float global_animation_timer = 0.0f;
static float screen_shake = 0.0f;
static float background_pulse = 0.0f;
static float danger_flash = 0.0f;

static void spawn_particle_burst(float x, float y, int count, ALLEGRO_COLOR color);

static void init_arrays(void)
{
    for (int i = 0; i < MAX_DEFENDERS; i++) defenders[i].base.active = false;
    for (int i = 0; i < MAX_PROJECTILES; i++) projectiles[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0.0f;
    for (int i = 0; i < MAX_ORBS; i++) orbs[i].active = false;
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) boss_projectiles[i].active = false;

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            grid_cells[row][col].hue = (float)(row * GRID_COLS + col) / (GRID_ROWS * GRID_COLS);
            grid_cells[row][col].pulse = (float)(rand() % 100) / 100.0f * 3.14159f * 2.0f;
            grid_cells[row][col].shake_x = 0.0f;
            grid_cells[row][col].shake_y = 0.0f;
        }
    }

    boss.state = BOSS_COUNTDOWN;
    boss.countdown = 10;
    boss.countdown_timer = 0.0f;
    boss.max_health = 150;
    boss.health = 150;
    boss.attack_timer = 0.0f;
    boss.attack_cooldown = 2.0f;
    boss.laser_row = -1;
    boss.laser_timer = 0.0f;
    boss.laser_duration = 1.5f;
    boss.laser_active = false;
    boss.enter_progress = 0.0f;
}

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
            int max_x = screen_width_cached - (int)(cell_width * 2.5f);
            orbs[i].x = 100.0f + (rand() % (max_x > 200 ? max_x - 100 : 1));
            orbs[i].y = -30.0f;
            orbs[i].vy = 80.0f + (rand() % 40);
            orbs[i].rotation = 0.0f;
            break;
        }
    }
}

static void spawn_boss_fireball(void)
{
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        if (!boss_projectiles[i].active)
        {
            boss_projectiles[i].active = true;
            boss_projectiles[i].x = boss.x - 50.0f;

            int target_row = rand() % GRID_ROWS;
            int target_col = rand() % (GRID_COLS - 2);

            boss_projectiles[i].target_row = target_row;
            boss_projectiles[i].target_col = target_col;

            float target_x = target_col * (cell_width + 1.0f) + cell_width / 2.0f;
            float target_y = GRID_START_Y + target_row * cell_height + cell_height / 2.0f;

            boss_projectiles[i].y = boss.y;

            float dx = target_x - boss_projectiles[i].x;
            float dy = target_y - boss_projectiles[i].y;
            float dist = sqrt(dx * dx + dy * dy);

            float speed = 280.0f;
            boss_projectiles[i].vx = (dx / dist) * speed;
            boss_projectiles[i].vy = (dy / dist) * speed;

            break;
        }
    }
}

static void start_boss_laser(void)
{
    boss.laser_active = true;
    boss.laser_row = rand() % GRID_ROWS;
    boss.laser_timer = 0.0f;
    screen_shake = 0.3f;
}

static void add_defender(int row, int col, int defender_id)
{
    const Defender* defender = get_equipped_defender(defender_id);

    if (!defender || placement_cooldown > 0.0f)
        return;

    int cost = defender->cost_to_place;

    if (vitamins < cost)
        return;

    if (col >= GRID_COLS - 2)
        return;

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].base.active && defenders[i].base.row == row &&
            defenders[i].base.col == col)
            return;
    }

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (!defenders[i].base.active)
        {
            defenders[i].base.active = true;
            defenders[i].base.row = row;
            defenders[i].base.col = col;
            defenders[i].base.speed = 0.0f;
            defenders[i].base.id = defender_id;

            placement_cooldown = 1.0f;
            vitamins -= cost;
            selected_defender = -1;
            break;
        }
    }
}

static void shoot_projectile(int row, int col, int defender_id)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!projectiles[i].active)
        {
            const Defender* defender = get_equipped_defender(defender_id);

            projectiles[i].active = true;
            projectiles[i].row = row;
            projectiles[i].x = col * (cell_width + 1.0f) + cell_width;
            projectiles[i].y = GRID_START_Y + row * cell_height + cell_height / 2.0f;
            projectiles[i].animation_time = 0.0f;
            projectiles[i].damage = defender->base.attack;

            if (defender_id == 0)
            {
                projectiles[i].type = PROJECTILE_WHITE_BALL;
                projectiles[i].speed = 180.0f;
            }
            else if (defender_id == 1)
            {
                projectiles[i].type = PROJECTILE_BLUE_MAGIC;
                projectiles[i].speed = 240.0f;
            }
            else
            {
                projectiles[i].type = PROJECTILE_RED_LASER;
                projectiles[i].speed = 320.0f;
            }
            break;
        }
    }
}

static void update_defenders(void)
{
    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].base.active)
        {
            const Defender* defender = get_equipped_defender(defenders[i].base.id);

            if (!defender)
                continue;

            defenders[i].base.speed += delta_time;

            float cooldown = (defender->base.id >= 0 && defender->base.id < 3)
                                 ? defender->base.attack_cooldown
                                 : 3.0f;

            if (defenders[i].base.speed >= cooldown)
            {
                if (boss.state == BOSS_ACTIVE)
                {
                    shoot_projectile(defenders[i].base.row, defenders[i].base.col,
                                     defender->base.id);
                }
                defenders[i].base.speed = 0.0f;
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

            if (boss.state == BOSS_ACTIVE)
            {
                if (projectiles[i].x >= boss.x - cell_width)
                {
                    boss.health -= projectiles[i].damage;
                    spawn_particle_burst(projectiles[i].x, projectiles[i].y, 10,
                                         al_map_rgb(255, 100, 100));
                    projectiles[i].active = false;
                    screen_shake = 0.1f;

                    if (boss.health <= 0)
                    {
                        boss.health = 0;
                        boss.state = BOSS_DEFEATED;
                        stage_complete = true;
                        screen_shake = 1.0f;

                        for (int p = 0; p < 100; p++)
                        {
                            float px = boss.x - (rand() % 150);
                            float py = GRID_START_Y + (rand() % (int)(cell_height * GRID_ROWS));
                            spawn_particle_burst(px, py, 15,
                                                 al_map_rgb(255, 50 + rand() % 100, 50));
                        }
                    }
                }
            }
        }
    }
}

static void update_boss_projectiles(void)
{
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        if (boss_projectiles[i].active)
        {
            boss_projectiles[i].x += boss_projectiles[i].vx * delta_time;
            boss_projectiles[i].y += boss_projectiles[i].vy * delta_time;

            float target_x =
                boss_projectiles[i].target_col * (cell_width + 1.0f) + cell_width / 2.0f;
            float target_y =
                GRID_START_Y + boss_projectiles[i].target_row * cell_height + cell_height / 2.0f;

            float dx = boss_projectiles[i].x - target_x;
            float dy = boss_projectiles[i].y - target_y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist < 30.0f)
            {
                spawn_particle_burst(boss_projectiles[i].x, boss_projectiles[i].y, 20,
                                     al_map_rgb(255, 50, 50));

                grid_cells[boss_projectiles[i].target_row][boss_projectiles[i].target_col].shake_x =
                    10.0f;
                grid_cells[boss_projectiles[i].target_row][boss_projectiles[i].target_col].shake_y =
                    10.0f;

                for (int j = 0; j < MAX_DEFENDERS; j++)
                {
                    if (defenders[j].base.active &&
                        defenders[j].base.row == boss_projectiles[i].target_row &&
                        defenders[j].base.col == boss_projectiles[i].target_col)
                    {
                        float def_x =
                            defenders[j].base.col * (cell_width + 1.0f) + cell_width / 2.0f;
                        float def_y =
                            GRID_START_Y + defenders[j].base.row * cell_height + cell_height / 2.0f;
                        spawn_particle_burst(def_x, def_y, 25, al_map_rgb(200, 50, 50));
                        defenders[j].base.active = false;
                        break;
                    }
                }

                boss_projectiles[i].active = false;
            }

            if (boss_projectiles[i].x < -50.0f ||
                boss_projectiles[i].x > screen_width_cached + 50.0f ||
                boss_projectiles[i].y < -50.0f ||
                boss_projectiles[i].y > screen_height_cached + 50.0f)
            {
                boss_projectiles[i].active = false;
            }
        }
    }
}

static void update_boss_laser(void)
{
    if (boss.laser_active)
    {
        boss.laser_timer += delta_time;

        if (boss.laser_timer >= 0.5f && boss.laser_timer < 0.6f)
        {
            for (int i = 0; i < MAX_DEFENDERS; i++)
            {
                if (defenders[i].base.active && defenders[i].base.row == boss.laser_row)
                {
                    float def_x = defenders[i].base.col * (cell_width + 1.0f) + cell_width / 2.0f;
                    float def_y =
                        GRID_START_Y + defenders[i].base.row * cell_height + cell_height / 2.0f;
                    spawn_particle_burst(def_x, def_y, 30, al_map_rgb(255, 100, 100));
                    defenders[i].base.active = false;
                }
            }

            for (int col = 0; col < GRID_COLS; col++)
            {
                grid_cells[boss.laser_row][col].shake_x = 15.0f;
                grid_cells[boss.laser_row][col].shake_y = 15.0f;
            }

            screen_shake = 0.5f;
        }

        if (boss.laser_timer >= boss.laser_duration)
        {
            boss.laser_active = false;
            boss.laser_timer = 0.0f;
        }
    }
}

static void update_boss(void)
{
    switch (boss.state)
    {
        case BOSS_COUNTDOWN:
            boss.countdown_timer += delta_time;
            danger_flash = sin(game_time * 8.0f) * 0.3f + 0.3f;

            if (boss.countdown_timer >= 1.0f)
            {
                boss.countdown--;
                boss.countdown_timer = 0.0f;
                screen_shake = 0.1f;

                if (boss.countdown <= 0)
                {
                    boss.state = BOSS_ENTERING;
                    boss.enter_progress = 0.0f;
                    boss.x = (float)screen_width_cached + cell_width;
                    boss.y = GRID_START_Y + (GRID_ROWS * cell_height) / 2.0f;
                    screen_shake = 0.5f;
                }
            }
            break;

        case BOSS_ENTERING:
            boss.enter_progress += delta_time * 0.4f;
            danger_flash = sin(game_time * 10.0f) * 0.4f + 0.4f;

            float target_x = screen_width_cached - cell_width * 1.5f;
            boss.x = screen_width_cached + cell_width -
                     (screen_width_cached + cell_width - target_x) * boss.enter_progress;

            if (boss.enter_progress >= 1.0f)
            {
                boss.x = target_x;
                boss.state = BOSS_ACTIVE;
                boss.attack_timer = 0.0f;
                screen_shake = 0.8f;
                danger_flash = 0.0f;
            }
            break;

        case BOSS_ACTIVE:
            boss.attack_timer += delta_time;

            update_boss_laser();

            if (!boss.laser_active && boss.attack_timer >= boss.attack_cooldown)
            {
                int attack_type = rand() % 10;

                if (attack_type < 6)
                {
                    int num_fireballs = 3 + (rand() % 3);
                    for (int i = 0; i < num_fireballs; i++)
                    {
                        spawn_boss_fireball();
                    }
                    screen_shake = 0.2f;
                }
                else
                {
                    start_boss_laser();
                }

                boss.attack_timer = 0.0f;
            }

            update_boss_projectiles();
            break;

        case BOSS_DEFEATED:
        case BOSS_INACTIVE:
        default:
            break;
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

static void update_grid_cells(void)
{
    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            grid_cells[row][col].pulse += delta_time * (2.0f + (float)(row + col) * 0.3f);

            if (grid_cells[row][col].shake_x > 0)
                grid_cells[row][col].shake_x -= delta_time * 30.0f;
            if (grid_cells[row][col].shake_y > 0)
                grid_cells[row][col].shake_y -= delta_time * 30.0f;

            if (boss.state == BOSS_ACTIVE)
            {
                grid_cells[row][col].hue += delta_time * 0.1f;
                if (grid_cells[row][col].hue > 1.0f)
                    grid_cells[row][col].hue -= 1.0f;
            }
        }
    }
}

static void init(ALLEGRO_DISPLAY* display)
{
    current_game_state = GAME_PLAYING;
    srand((unsigned int)time(NULL));
    init_arrays();

    game_time = 0.0f;
    vitamins = 300;
    stage_complete = false;
    stage_failed = false;
    global_animation_timer = 0.0f;
    screen_shake = 0.0f;
    background_pulse = 0.0f;
    danger_flash = 0.0f;

    boss_image = al_load_bitmap("assets/images/enemies/boss.png");
    if (boss_image)
    {
        boss_w = al_get_bitmap_width(boss_image);
        boss_h = al_get_bitmap_height(boss_image);
    }

    font = al_create_builtin_font();
    title_font = al_create_builtin_font();
    countdown_font = al_create_builtin_font();

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
            if (stage_complete)
            {
                Player->vaccines += 100;

                if (Player->current_stage == 11)
                {
                    Player->current_stage++;
                }
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

        if (mouse_y < SELECTOR_HEIGHT && boss.state == BOSS_ACTIVE)
        {
            int selector_width = 100;
            int start_x = 10;

            for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
            {
                const Defender* defender = get_equipped_defender(Player->in_use_slots[i]);

                if (!defender)
                    continue;

                int x1 = start_x + i * (selector_width + 10);
                int x2 = x1 + selector_width;
                if (mouse_x >= x1 && mouse_x <= x2 && mouse_y >= 10 &&
                    mouse_y <= SELECTOR_HEIGHT - 10)
                {
                    if (vitamins >= defender->cost_to_place)
                        selected_defender = defender->base.id;
                    else
                        selected_defender = -1;
                    break;
                }
            }
            return;
        }

        if (mouse_y >= GRID_START_Y && boss.state == BOSS_ACTIVE)
        {
            int col = (int)(mouse_x / (cell_width + 1.0f));
            int row = (int)((mouse_y - GRID_START_Y) / cell_height);

            if (col >= GRID_COLS - 2)
                return;

            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS)
            {
                if (selected_defender != -1)
                    add_defender(row, col, selected_defender);
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
        global_animation_timer += delta_time;
        background_pulse += delta_time * 2.0f;

        if (screen_shake > 0)
            screen_shake -= delta_time * 3.0f;
        if (screen_shake < 0)
            screen_shake = 0;

        if (placement_cooldown > 0.0f)
            placement_cooldown -= delta_time;

        update_grid_cells();
        update_defenders();
        update_projectiles(screen_width_cached);
        update_particles();
        update_orbs();
        update_boss();

        if (boss.state == BOSS_ACTIVE && rand() % 300 < 2)
        {
            spawn_orb();
        }
    }
}

static ALLEGRO_COLOR hsv_to_rgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h * 6.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;
    if (h < 1.0f / 6.0f)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 2.0f / 6.0f)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 3.0f / 6.0f)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 4.0f / 6.0f)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 5.0f / 6.0f)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    return al_map_rgb((int)((r + m) * 255), (int)((g + m) * 255), (int)((b + m) * 255));
}

static void draw_animated_grid(int screen_width, int screen_height)
{
    float shake_offset_x = 0;
    float shake_offset_y = 0;

    if (screen_shake > 0)
    {
        shake_offset_x = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
        shake_offset_y = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
    }

    float bg_intensity = 0.15f + sin(background_pulse) * 0.05f;
    int bg_r = (int)(40 * bg_intensity + danger_flash * 100);
    int bg_g = (int)(20 * bg_intensity);
    int bg_b = (int)(60 * bg_intensity);
    al_clear_to_color(al_map_rgb(bg_r > 255 ? 255 : bg_r, bg_g, bg_b));

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            float x = col * (cell_width + 1.0f) + shake_offset_x;
            float y = GRID_START_Y + row * cell_height + shake_offset_y;

            float cell_shake_x = 0;
            float cell_shake_y = 0;
            if (grid_cells[row][col].shake_x > 0)
            {
                cell_shake_x =
                    ((float)(rand() % 100) / 100.0f - 0.5f) * grid_cells[row][col].shake_x;
                cell_shake_y =
                    ((float)(rand() % 100) / 100.0f - 0.5f) * grid_cells[row][col].shake_y;
            }

            x += cell_shake_x;
            y += cell_shake_y;

            float pulse_val = sin(grid_cells[row][col].pulse) * 0.5f + 0.5f;
            float hue = grid_cells[row][col].hue;

            ALLEGRO_COLOR cell_color;
            if (boss.state == BOSS_ACTIVE)
            {
                float intensity = 0.3f + pulse_val * 0.4f;
                cell_color = hsv_to_rgb(hue, 0.6f, intensity);
            }
            else if (boss.state == BOSS_COUNTDOWN || boss.state == BOSS_ENTERING)
            {
                float red_pulse = sin(game_time * 5.0f + row + col) * 0.3f + 0.3f;
                cell_color = al_map_rgb((int)(80 + red_pulse * 100), 20, 40);
            }
            else
            {
                cell_color = al_map_rgb(40, 40, 60);
            }

            al_draw_filled_rectangle(x, y, x + cell_width - 2, y + cell_height - 2, cell_color);

            float border_pulse =
                sin(global_animation_timer * 3.0f + row * 0.5f + col * 0.3f) * 0.5f + 0.5f;
            int border_intensity = (int)(100 + border_pulse * 100);
            ALLEGRO_COLOR border_color =
                al_map_rgb(border_intensity, border_intensity / 2, border_intensity);
            al_draw_rectangle(x, y, x + cell_width - 2, y + cell_height - 2, border_color, 2.0f);
        }
    }
}

static void draw_boss(int screen_width, int screen_height)
{
    float shake_offset_x = 0;
    float shake_offset_y = 0;

    if (screen_shake > 0)
    {
        shake_offset_x = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
        shake_offset_y = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
    }

    if (boss.state == BOSS_COUNTDOWN)
    {
        float flash = sin(game_time * 10.0f) * 0.5f + 0.5f;
        al_draw_filled_rectangle(screen_width / 2 - 150, screen_height / 2 - 100,
                                 screen_width / 2 + 150, screen_height / 2 + 100,
                                 al_map_rgba(50 + (int)(flash * 50), 0, 0, 220));

        char countdown_text[16];
        sprintf(countdown_text, "%d", boss.countdown);

        ALLEGRO_TRANSFORM transform;
        al_identity_transform(&transform);

        float scale = 6.0f + sin(game_time * 8.0f) * 0.5f;
        al_translate_transform(&transform,
                               -al_get_text_width(countdown_font, countdown_text) / 2.0f,
                               -al_get_font_line_height(countdown_font) / 2.0f);
        al_scale_transform(&transform, scale, scale);
        al_translate_transform(&transform, screen_width / 2 + shake_offset_x,
                               screen_height / 2 + shake_offset_y);
        al_use_transform(&transform);

        al_draw_text(countdown_font, al_map_rgb(255, 50 + (int)(flash * 100), 50), 0, 0, 0,
                     countdown_text);

        al_identity_transform(&transform);
        al_use_transform(&transform);

        float text_flash = sin(game_time * 6.0f) * 0.5f + 0.5f;
        al_draw_text(font,
                     al_map_rgb(255, (int)(150 + text_flash * 105), (int)(150 + text_flash * 105)),
                     screen_width / 2, screen_height / 2 + 70, ALLEGRO_ALIGN_CENTER,
                     "O BOSS ESTA CHEGANDO!");
    }

    if (boss.state == BOSS_ENTERING || boss.state == BOSS_ACTIVE || boss.state == BOSS_DEFEATED)
    {
        float boss_draw_width = cell_width * 2.0f;
        float boss_draw_height = cell_height * GRID_ROWS;
        float boss_draw_x = boss.x - boss_draw_width / 2.0f + shake_offset_x;
        float boss_draw_y = GRID_START_Y + shake_offset_y;

        if (boss.state == BOSS_ACTIVE)
        {
            float glow_pulse = sin(game_time * 4.0f) * 0.3f + 0.7f;
            al_draw_filled_rectangle(boss_draw_x - 10, boss_draw_y - 10,
                                     boss_draw_x + boss_draw_width + 10,
                                     boss_draw_y + boss_draw_height + 10,
                                     al_map_rgba((int)(150 * glow_pulse), 0, 0, 100));
        }

        if (boss_image)
        {
            al_draw_scaled_bitmap(boss_image, 0, 0, boss_w, boss_h, boss_draw_x, boss_draw_y,
                                  boss_draw_width, boss_draw_height, 0);
        }
        else
        {
            al_draw_filled_rectangle(boss_draw_x, boss_draw_y, boss_draw_x + boss_draw_width,
                                     boss_draw_y + boss_draw_height, al_map_rgb(150, 0, 150));

            ALLEGRO_TRANSFORM t;
            al_identity_transform(&t);
            al_translate_transform(&t, -al_get_text_width(font, "BOSS") / 2.0f, -8);
            al_scale_transform(&t, 2.0f, 2.0f);
            al_translate_transform(&t, boss_draw_x + boss_draw_width / 2,
                                   boss_draw_y + boss_draw_height / 2);
            al_use_transform(&t);
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "BOSS");
            al_identity_transform(&t);
            al_use_transform(&t);
        }

        if (boss.state == BOSS_ACTIVE)
        {
            float bar_width = 400.0f;
            float bar_height = 30.0f;
            float bar_x = (screen_width - bar_width) / 2.0f;
            float bar_y = 15.0f;

            al_draw_filled_rectangle(bar_x - 3, bar_y - 3, bar_x + bar_width + 3,
                                     bar_y + bar_height + 3, al_map_rgb(30, 30, 30));

            al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height,
                                     al_map_rgb(60, 0, 0));

            float health_percent = (float)boss.health / (float)boss.max_health;
            float bar_pulse = sin(game_time * 5.0f) * 20.0f;
            int red_val = 200 + (int)bar_pulse;
            al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_width * health_percent,
                                     bar_y + bar_height,
                                     al_map_rgb(red_val > 255 ? 255 : red_val, 20, 20));

            al_draw_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height,
                              al_map_rgb(255, 255, 255), 3.0f);

            char health_text[32];
            sprintf(health_text, "VILAO FINAL: %d/%d", boss.health, boss.max_health);
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, bar_y + 7,
                         ALLEGRO_ALIGN_CENTER, health_text);
        }
    }

    if (boss.laser_active && boss.state == BOSS_ACTIVE)
    {
        float laser_y =
            GRID_START_Y + boss.laser_row * cell_height + cell_height / 2.0f + shake_offset_y;

        if (boss.laser_timer < 0.3f)
        {
            float alpha = boss.laser_timer / 0.3f;
            float warning_pulse = sin(game_time * 30.0f) * 0.5f + 0.5f;
            al_draw_line(0, laser_y, boss.x + shake_offset_x, laser_y,
                         al_map_rgba(255, (int)(100 * warning_pulse), 0, (int)(150 * alpha)), 4.0f);
        }
        else if (boss.laser_timer < 0.5f)
        {
            float pulse = sin((boss.laser_timer - 0.3f) * 80.0f) * 0.5f + 0.5f;
            al_draw_filled_rectangle(0, laser_y - 8 - pulse * 15, boss.x + shake_offset_x,
                                     laser_y + 8 + pulse * 15, al_map_rgba(255, 150, 150, 180));
            al_draw_line(0, laser_y, boss.x + shake_offset_x, laser_y, al_map_rgb(255, 50, 50),
                         6.0f);
        }
        else
        {
            float fade = 1.0f - (boss.laser_timer - 0.5f) / (boss.laser_duration - 0.5f);
            al_draw_filled_rectangle(0, laser_y - 25 * fade, boss.x + shake_offset_x,
                                     laser_y + 25 * fade,
                                     al_map_rgba(255, 200, 200, (int)(180 * fade)));
            al_draw_filled_rectangle(0, laser_y - 12 * fade, boss.x + shake_offset_x,
                                     laser_y + 12 * fade,
                                     al_map_rgba(255, 50, 50, (int)(255 * fade)));
            al_draw_line(0, laser_y, boss.x + shake_offset_x, laser_y,
                         al_map_rgba(255, 255, 255, (int)(255 * fade)), 4.0f);
        }
    }

    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
    {
        if (boss_projectiles[i].active)
        {
            float px = boss_projectiles[i].x + shake_offset_x;
            float py = boss_projectiles[i].y + shake_offset_y;
            float fireball_pulse = sin(game_time * 20.0f + i) * 3.0f;

            al_draw_filled_circle(px, py, 18.0f + fireball_pulse, al_map_rgba(255, 50, 50, 150));
            al_draw_filled_circle(px, py, 12.0f + fireball_pulse * 0.5f, al_map_rgb(255, 100, 50));
            al_draw_filled_circle(px, py, 6.0f, al_map_rgb(255, 200, 100));
        }
    }
}

static void draw_game_elements(int screen_width, int screen_height)
{
    float shake_offset_x = 0;
    float shake_offset_y = 0;

    if (screen_shake > 0)
    {
        shake_offset_x = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
        shake_offset_y = ((float)(rand() % 100) / 100.0f - 0.5f) * screen_shake * 20.0f;
    }

    if (boss.state == BOSS_ACTIVE)
    {
        int selector_width = 100;
        int start_x = 10;

        for (int i = 0; i < MAX_IN_USE_SLOTS; i++)
        {
            const Defender* defender = get_equipped_defender(Player->in_use_slots[i]);

            if (!defender)
                continue;

            int x1 = start_x + i * (selector_width + 10);
            int x2 = x1 + selector_width;
            ALLEGRO_COLOR color;
            ALLEGRO_COLOR border_color;
            ALLEGRO_COLOR text_color;

            if (vitamins >= defender->cost_to_place)
            {
                color = (selected_defender == defender->base.id) ? al_map_rgba(0, 255, 0, 180)
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

            if (defender->base.image)
            {
                al_draw_scaled_bitmap(defender->base.image, 0, 0, defender->base.image_width,
                                      defender->base.image_height, x1 + 35, 15, 35, 35, 0);
            }

            char cost_text[16];
            sprintf(cost_text, "C:%d", defender->cost_to_place);
            if (font)
                al_draw_text(font, text_color, x1 + 50, 50, ALLEGRO_ALIGN_CENTER, cost_text);
        }

        char vitamins_text[32];
        sprintf(vitamins_text, "Vitaminas: %d", vitamins);
        al_draw_filled_rectangle(screen_width - 160, 55, screen_width - 10, 85,
                                 al_map_rgba(0, 0, 0, 180));
        if (font)
            al_draw_text(font, al_map_rgb(255, 215, 0), screen_width - 85, 63, ALLEGRO_ALIGN_CENTER,
                         vitamins_text);
    }

    for (int i = 0; i < MAX_DEFENDERS; i++)
    {
        if (defenders[i].base.active)
        {
            const Defender* defender = get_equipped_defender(defenders[i].base.id);

            if (!defender)
                continue;

            float x = defenders[i].base.col * (cell_width + 1.0f) + shake_offset_x;
            float y = GRID_START_Y + defenders[i].base.row * cell_height + shake_offset_y;

            float cell_shake_x = grid_cells[defenders[i].base.row][defenders[i].base.col].shake_x;
            float cell_shake_y = grid_cells[defenders[i].base.row][defenders[i].base.col].shake_y;
            if (cell_shake_x > 0)
                x += ((float)(rand() % 100) / 100.0f - 0.5f) * cell_shake_x;
            if (cell_shake_y > 0)
                y += ((float)(rand() % 100) / 100.0f - 0.5f) * cell_shake_y;

            float scale = (defender->base.image_width > 0)
                              ? (cell_width * 0.8f) / defender->base.image_width
                              : 1.0f;
            float img_w = defender->base.image_width * scale;
            float img_h = defender->base.image_height * scale;

            if (defender->base.image)
            {
                al_draw_scaled_bitmap(defender->base.image, 0, 0, defender->base.image_width,
                                      defender->base.image_height, x + (cell_width - img_w) / 2.0f,
                                      y + (cell_height - 10.0f - img_h) / 2.0f, img_w, img_h, 0);
            }
        }
    }

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i].active)
        {
            float px = projectiles[i].x + shake_offset_x;
            float py = projectiles[i].y + shake_offset_y;
            float anim = projectiles[i].animation_time;

            if (projectiles[i].type == PROJECTILE_WHITE_BALL)
            {
                float pulse = sin(anim * 10.0f) * 2.0f;
                al_draw_filled_circle(px, py, 12.0f + pulse, al_map_rgb(255, 255, 255));
            }
            else if (projectiles[i].type == PROJECTILE_BLUE_MAGIC)
            {
                al_draw_filled_circle(px, py, 10.0f, al_map_rgb(100, 200, 255));
            }
            else if (projectiles[i].type == PROJECTILE_RED_LASER)
            {
                al_draw_filled_rectangle(px - 25, py - 4, px + 25, py + 4, al_map_rgb(255, 30, 30));
            }
        }
    }

    for (int i = 0; i < MAX_ORBS; i++)
    {
        if (orbs[i].active)
        {
            float orb_pulse = sin(game_time * 8.0f + i) * 3.0f;
            al_draw_filled_circle(orbs[i].x + shake_offset_x, orbs[i].y + shake_offset_y,
                                  15.0f + orb_pulse, al_map_rgb(255, 215, 0));
            al_draw_filled_circle(orbs[i].x + shake_offset_x, orbs[i].y + shake_offset_y, 8.0f,
                                  al_map_rgb(255, 255, 200));
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].life > 0.0f)
        {
            float alpha_factor = particles[i].life / particles[i].max_life;
            ALLEGRO_COLOR particle_color = particles[i].color;
            particle_color.a = alpha_factor;
            al_draw_filled_circle(particles[i].x + shake_offset_x, particles[i].y + shake_offset_y,
                                  3.0f + (1.0f - alpha_factor) * 2.0f, particle_color);
        }
    }
}

static void draw(int screen_width, int screen_height)
{
    if (screen_width_cached != screen_width || screen_height_cached != screen_height)
    {
        screen_width_cached = screen_width;
        screen_height_cached = screen_height;
        cell_width = ((float)screen_width / GRID_COLS) - 5.0f;
        cell_height = (float)(screen_height - GRID_START_Y) / GRID_ROWS;
    }

    draw_animated_grid(screen_width, screen_height);
    draw_game_elements(screen_width, screen_height);
    draw_boss(screen_width, screen_height);

    if (stage_complete)
    {
        al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 200));

        float victory_pulse = sin(game_time * 5.0f) * 0.2f + 1.0f;

        ALLEGRO_TRANSFORM t;
        al_identity_transform(&t);
        al_translate_transform(&t, -al_get_text_width(title_font, "PARABENS!") / 2.0f, -8);
        al_scale_transform(&t, 3.0f * victory_pulse, 3.0f * victory_pulse);
        al_translate_transform(&t, screen_width / 2, screen_height / 2 - 120);
        al_use_transform(&t);
        al_draw_text(title_font, al_map_rgb(255, 215, 0), 0, 0, 0, "PARABENS!");
        al_identity_transform(&t);
        al_use_transform(&t);

        al_identity_transform(&t);
        al_translate_transform(&t, -al_get_text_width(title_font, "VOCE DERROTOU O VILAO!") / 2.0f,
                               -8);
        al_scale_transform(&t, 2.0f, 2.0f);
        al_translate_transform(&t, screen_width / 2, screen_height / 2 - 40);
        al_use_transform(&t);
        al_draw_text(title_font, al_map_rgb(0, 255, 0), 0, 0, 0, "VOCE DERROTOU O VILAO!");
        al_identity_transform(&t);
        al_use_transform(&t);

        al_identity_transform(&t);
        al_translate_transform(&t, -al_get_text_width(font, "O corpo esta protegido!") / 2.0f, -8);
        al_scale_transform(&t, 1.5f, 1.5f);
        al_translate_transform(&t, screen_width / 2, screen_height / 2 + 30);
        al_use_transform(&t);
        al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "O corpo esta protegido!");
        al_identity_transform(&t);
        al_use_transform(&t);

        al_draw_text(font, al_map_rgb(255, 215, 0), screen_width / 2, screen_height / 2 + 80,
                     ALLEGRO_ALIGN_CENTER, "Bonus de vitoria: +100 Vacinas!");

        float blink = sin(game_time * 4.0f) * 0.5f + 0.5f;
        al_draw_text(font,
                     al_map_rgb((int)(150 + blink * 105), (int)(150 + blink * 105),
                                (int)(150 + blink * 105)),
                     screen_width / 2, screen_height / 2 + 130, ALLEGRO_ALIGN_CENTER,
                     "Pressione qualquer tecla");
    }

    if (stage_failed)
    {
        al_draw_filled_rectangle(0, 0, screen_width, screen_height, al_map_rgba(0, 0, 0, 200));

        ALLEGRO_TRANSFORM t;
        al_identity_transform(&t);
        al_translate_transform(&t, -al_get_text_width(title_font, "DERROTA!") / 2.0f, -8);
        al_scale_transform(&t, 3.0f, 3.0f);
        al_translate_transform(&t, screen_width / 2, screen_height / 2 - 50);
        al_use_transform(&t);
        al_draw_text(title_font, al_map_rgb(255, 0, 0), 0, 0, 0, "DERROTA!");
        al_identity_transform(&t);
        al_use_transform(&t);

        al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2 + 30,
                     ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla");
    }
}

static void destroy(void)
{
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

    if (countdown_font)
    {
        al_destroy_font(countdown_font);
        countdown_font = NULL;
    }

    if (boss_image)
    {
        al_destroy_bitmap(boss_image);
        boss_image = NULL;
        boss_w = boss_h = 0;
    }
}

Screen BossStageScreen = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
