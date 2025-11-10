#include "bestiary.h"

static ALLEGRO_BITMAP* background = NULL;

static int current_creature_index = 0;
static int* microorganisms_indexes_memo = NULL;

static void on_back_screen()
{
    current_screen->destroy();
    current_screen = &LobbyScreen;
    current_screen->init(NULL);
}

static void on_microorganism_change(void* context)
{
    int index = *(int*)context;

    current_creature_index = index;
}

static void init(ALLEGRO_DISPLAY* display)
{
    load_microorganisms();

    background = al_load_bitmap("assets/images/menu/background_bestiary.png");

    microorganisms_indexes_memo = malloc(sizeof(int) * total_microorganisms);
    if (!microorganisms_indexes_memo)
    {
        fprintf(stderr, "Erro ao alocar memória.\n");

        exit(1);
    }

    current_creature_index = 0;
}

static void update(ALLEGRO_EVENT* event, bool* running) {}

static void draw(int screen_width, int screen_height)
{
    al_clear_to_color(COLOR_WHITE);

    if (background)
    {
        al_draw_scaled_bitmap(background, 0, 0, al_get_bitmap_width(background),
                              al_get_bitmap_height(background), 0, 0, screen_width, screen_height,
                              0);
    }

    Button back_button = {
        .x = 70,
        .y = screen_height - 300,
        .width = 160,
        .height = 75,
        .fill_color = &COLOR_YELLOW,
        .text = {.content = "Voltar", .color = &COLOR_BLACK, .font = fonts[FONT_H2]},
        .border = {.border_color = &COLOR_BLACK, .thickness = 2},
        .on_click = on_back_screen,
        .context = NULL,
    };

    draw_button(&back_button);

    float divider_x = screen_width / 2;
    al_draw_line(divider_x, 0, divider_x, screen_height, COLOR_BLACK, 2);

    float padding = 200;
    float button_spacing_x = 15;
    float button_spacing_y = 15;
    float button_width = 150;
    float button_height = 50;
    int buttons_per_row = 2;

    for (int i = 0; i < total_microorganisms; i++)
    {
        microorganisms_indexes_memo[i] = i;

        int row = i / buttons_per_row;
        int col = i % buttons_per_row;
        float button_x = padding + col * (button_width + button_spacing_x);
        float button_y = padding + row * (button_height + button_spacing_y);

        Entity* entity;
        ALLEGRO_COLOR* fill_color;
        if (microorganisms[i].is_defender)
        {
            entity = &((ImmuneCell*)microorganisms[i].entity)->base;
            fill_color = &COLOR_DEFENDER_BUTTON;
        }
        else
        {
            entity = &((Pathogen*)microorganisms[i].entity)->base;
            fill_color = &COLOR_ENEMY_BUTTON;
        }

        Button button = {
            .x = button_x,
            .y = button_y,
            .width = button_width,
            .height = button_height,
            .text = {.content = entity->name, .color = &COLOR_WHITE, .font = fonts[FONT_NORMAL]},
            .fill_color = fill_color,
            .border = {.border_color = &COLOR_BLACK, .thickness = 2},
            .on_click = on_microorganism_change,
            .context = &microorganisms_indexes_memo[i],
        };

        draw_button(&button);
    }

    if (current_creature_index >= 0 && current_creature_index < total_microorganisms)
    {
        Microorganism* current = &microorganisms[current_creature_index];
        Entity* entity = current->entity;
        if (!entity)
        {
            return;
        }

        float image_x = divider_x + 225;
        float image_y = 275;
        float image_width = 200;
        float image_height = 200;

        if (entity->image)
        {
            al_draw_scaled_bitmap(entity->image, 0, 0, al_get_bitmap_width(entity->image),
                                  al_get_bitmap_height(entity->image), image_x, image_y,
                                  image_width, image_height, 0);
        }

        al_draw_text(fonts[FONT_H1], COLOR_YELLOW, image_x - 100, image_y + image_height + 40,
                     ALLEGRO_ALIGN_LEFT, entity->name);

        if (entity->description)
        {
            al_draw_multiline_text(fonts[FONT_H4], COLOR_WHITE, image_x - 100,
                                   image_y + image_height + 100, 400, 24, ALLEGRO_ALIGN_LEFT,
                                   entity->description);
        }
    }
}

static void destroy()
{
    if (background)
    {
        al_destroy_bitmap(background);
    }

    if (microorganisms_indexes_memo)
    {
        free(microorganisms_indexes_memo);
    }
}

Screen Bestiary = {
    .init = init,
    .update = update,
    .draw = draw,
    .destroy = destroy,
};
