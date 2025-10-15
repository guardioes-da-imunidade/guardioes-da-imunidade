#include "bestiary.h"

Enemy enemies[] = {
    {
        "Bactéria",
        "BactériaBactériaBactériaBactériaBactériaBactériaBactériaBactériaBactériaBactériaBactériaBa"
        "cté"
        "riaBactériaBactéria.",
        "bacterium",
        NULL,
    },
    {
        "Parasita",
        "ParasitaParasitaParasitaParasitaParasitaParasitaParasitaParasitaParasitaParasita.",
        "parasite",
        NULL,
    },
    {
        "Vírus",
        "VírusVírusVírusVírusVírusVírusVírusVírusVírusVírusVírusVírusVírusVírusVírus.",
        "virus",
        NULL,
    },
};

int total_enemies = sizeof(enemies) / sizeof(Enemy);
int current_enemy_index = 0;

static ALLEGRO_FONT *font = NULL;
static ALLEGRO_FONT *font_small = NULL;

static int mouse_x = 0;
static int mouse_y = 0;
static bool mouse_clicked = false;

static void load_enemy_images()
{
    for (int i = 0; i < total_enemies; i++)
    {
        char image_path[50];
        snprintf(image_path, sizeof(image_path), "assets/images/enemies/%s.png",
                 enemies[i].image_path);

        enemies[i].image = al_load_bitmap(image_path);

        if (!enemies[i].image)
        {
            printf("Falha ao carregar a imagem: %s\n", image_path);
        }
    }
}

static void init(ALLEGRO_DISPLAY *display)
{
    font = al_load_font("assets/fonts/arial.ttf", 24, 0);
    font_small = al_load_font("assets/fonts/arial.ttf", 12, 0);

    if (!font || !font_small)
    {
        font = al_create_builtin_font();
        font_small = al_create_builtin_font();
    }

    load_enemy_images();
}

static void update(ALLEGRO_EVENT *event, bool *running)
{
    if (event->type == ALLEGRO_EVENT_MOUSE_AXES)
    {
        mouse_x = event->mouse.x;
        mouse_y = event->mouse.y;
    }
    else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        mouse_clicked = true;
    }
    else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
    {
        mouse_clicked = false;
    }

    // TO-DO: Voltar uma tela
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        *running = false;
    }
}

void on_creature_change(void *context)
{
    int index = *(int *)context;

    current_enemy_index = index;
}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(al_map_rgb(251, 255, 255));

    float padding = 80;

    float divider_x = screen_width / 2;
    al_draw_line(divider_x, 0, divider_x, screen_height, COLOR_BLACK, 2);

    float button_width = 150;
    float button_height = 50;
    float button_spacing_x = 15;
    float button_spacing_y = 15;
    int buttons_per_row = 3;

    int indexes[total_enemies];
    for (int i = 0; i < total_enemies; i++)
    {
        indexes[i] = i;

        int row = i / buttons_per_row;
        int col = i % buttons_per_row;

        float button_x = padding + col * (button_width + button_spacing_x);
        float button_y = padding + row * (button_height + button_spacing_y);

        Button button = {
            .x = button_x,
            .y = button_y,
            .width = button_width,
            .height = button_height,
            .text = {.content = enemies[i].name, .color = &COLOR_BLACK, .font = font},
            .fill_color = &COLOR_YELLOW,
            .border =
                {
                    .border_color = &COLOR_BLACK,
                    .thickness = 1,
                },
            .on_click = on_creature_change,
            .context = &indexes[i],
        };

        draw_button(&button);
    }

    Enemy *current_enemy = &enemies[current_enemy_index];

    float right_area_x = divider_x + padding;
    float image_width = 250;
    float image_height = 250;
    float image_y = padding;

    if (current_enemy->image)
    {
        al_draw_scaled_bitmap(current_enemy->image, 0, 0, al_get_bitmap_width(current_enemy->image),
                              al_get_bitmap_height(current_enemy->image), right_area_x, image_y,
                              image_width, image_height, 0);
    }

    al_draw_text(font, COLOR_BLACK, right_area_x, image_y + image_height + padding,
                 ALLEGRO_ALIGN_LEFT, current_enemy->name);

    al_draw_text(font_small, COLOR_BLACK, right_area_x, image_y + image_height + padding + 40,
                 ALLEGRO_ALIGN_LEFT, current_enemy->description);
}

static void destroy()
{
    for (int i = 0; i < total_enemies; i++)
    {
        if (enemies[i].image)
        {
            al_destroy_bitmap(enemies[i].image);
        }
    }

    if (font)
        al_destroy_font(font);

    if (font_small)
        al_destroy_font(font_small);
}

Screen Bestiary = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
