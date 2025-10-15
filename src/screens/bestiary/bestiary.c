#include "bestiary.h"

static int current_creature_index = 0;
static ALLEGRO_FONT *font = NULL;
static ALLEGRO_FONT *font_small = NULL;

static void on_microorganism_change(void *context)
{
    int index = *(int *)context;

    current_creature_index = index;
}

static void init()
{
    font = al_load_font("assets/fonts/arial.ttf", 24, 0);
    font_small = al_load_font("assets/fonts/arial.ttf", 12, 0);

    if (!font || !font_small)
    {
        font = al_create_builtin_font();
        font_small = al_create_builtin_font();
    }

    load_microorganisms();
}

static void update() {}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(COLOR_WHITE);

    float padding = 80;
    float divider_x = screen_width / 2;
    al_draw_line(divider_x, 0, divider_x, screen_height, COLOR_BLACK, 2);

    float button_width = 150;
    float button_height = 50;
    float button_spacing_x = 15;
    float button_spacing_y = 15;
    int buttons_per_row = 3;

    int indexes[total_microorganisms];
    for (int i = 0; i < total_microorganisms; i++)
    {
        indexes[i] = i;

        int row = i / buttons_per_row;
        int col = i % buttons_per_row;
        float button_x = padding + col * (button_width + button_spacing_x);
        float button_y = padding + row * (button_height + button_spacing_y);

        Entity *entity;
        ALLEGRO_COLOR *fill_color;
        if (microorganisms[i].is_defender)
        {
            entity = &((Defender *)microorganisms[i].entity)->base;
            fill_color = &COLOR_DEFENDER_BUTTON;
        }
        else
        {
            entity = &((Enemy *)microorganisms[i].entity)->base;
            fill_color = &COLOR_ENEMY_BUTTON;
        }

        Button button = {
            .x = button_x,
            .y = button_y,
            .width = button_width,
            .height = button_height,
            .text = {.content = entity->name, .color = &COLOR_BLACK, .font = font},
            .fill_color = fill_color,
            .border = {.border_color = &COLOR_BLACK, .thickness = 2},
            .on_click = on_microorganism_change,
            .context = &indexes[i],
        };

        draw_button(&button);
    }

    Microorganism *current = &microorganisms[current_creature_index];
    Entity *entity = current->entity;

    float right_area_x = divider_x + padding;
    float image_width = 250;
    float image_height = 250;
    float image_y = padding;

    if (entity->image)
    {
        al_draw_scaled_bitmap(entity->image, 0, 0, al_get_bitmap_width(entity->image),
                              al_get_bitmap_height(entity->image), right_area_x, image_y,
                              image_width, image_height, 0);
    }

    al_draw_text(font, COLOR_BLACK, right_area_x, image_y + image_height + padding,
                 ALLEGRO_ALIGN_LEFT, entity->name);

    al_draw_text(font_small, COLOR_BLACK, right_area_x, image_y + image_height + padding + 40,
                 ALLEGRO_ALIGN_LEFT, entity->description);
}

static void destroy()
{
    for (int i = 0; i < total_microorganisms; i++)
    {
        if (microorganisms[i].entity->image)
            al_destroy_bitmap(microorganisms[i].entity->image);
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
