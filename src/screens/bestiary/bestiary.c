#include "bestiary.h"

static ALLEGRO_BITMAP* background = NULL;

static int current_creature_index = 0;
static int* microorganisms_indexes_memo = NULL;

static void on_equip_defender(void* context) { equip_defender((ImmuneCell*)context); }

static void on_unequip_defender(void* context) { unequip_defender((ImmuneCell*)context); }

static void on_buy_defender(void* context) { unlock_defender((ImmuneCell*)context); }

static void draw_stat_bar(const char* label, float value, float max_value, float x, float y,
                          float bar_width, float bar_height, ALLEGRO_COLOR color)
{
    float percent = value / max_value;
    if (percent > 1)
        percent = 1;

    float filled = bar_width * percent;

    al_draw_text(fonts[FONT_H5], COLOR_WHITE, x, y - 22, ALLEGRO_ALIGN_LEFT, label);

    al_draw_filled_rectangle(x, y, x + bar_width, y + bar_height, al_map_rgb(40, 40, 40));

    al_draw_filled_rectangle(x, y, x + filled, y + bar_height, color);

    al_draw_rectangle(x, y, x + bar_width, y + bar_height, COLOR_BLACK, 2);
}

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

        bool locked = false;

        if (microorganisms[i].is_defender)
        {
            ImmuneCell* def = (ImmuneCell*)microorganisms[i].entity;
            int id = def->defender_id;

            if (locked)
                fill_color = &COLOR_GRAY;
            else if (is_defender_equipped(id))
                fill_color = &COLOR_GREEN;
            else
                fill_color = &COLOR_BLUE;

            entity = &def->base;
        }
        else
        {
            entity = &((Pathogen*)microorganisms[i].entity)->base;
            fill_color = &COLOR_RED;
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
            al_draw_multiline_text(fonts[FONT_H5], COLOR_WHITE, image_x - 100,
                                   image_y + image_height + 100, 400, 24, ALLEGRO_ALIGN_LEFT,
                                   entity->description);
        }

        float stats_x = screen_width - 200;
        float stats_y = 50;
        float bar_width = 150;
        float bar_height = 12;

        draw_stat_bar("Vida", entity->health, 200, stats_x, stats_y, bar_width, bar_height,
                      al_map_rgb(0, 200, 0));

        draw_stat_bar("Ataque", entity->attack, 20, stats_x, stats_y + 50, bar_width, bar_height,
                      al_map_rgb(200, 0, 0));

        draw_stat_bar("Defesa", entity->defense, 20, stats_x, stats_y + 100, bar_width, bar_height,
                      al_map_rgb(0, 100, 255));

        draw_stat_bar("Velocidade", entity->speed, 2.0f, stats_x, stats_y + 150, bar_width,
                      bar_height, al_map_rgb(255, 220, 0));

        draw_stat_bar("Cooldown", 1.0f / entity->attack_cooldown, 2.0f, stats_x, stats_y + 200,
                      bar_width, bar_height, al_map_rgb(150, 0, 150));

        if (current->is_defender)
        {
            ImmuneCell* d = (ImmuneCell*)current->entity;
            bool unlocked = PLAYER_ENTITY->defenders[d->defender_id];
            bool equipped = is_defender_equipped(d->defender_id);

            draw_stat_bar("Custo", d->cost_to_unlock, 50, stats_x, stats_y + 250, bar_width,
                          bar_height, al_map_rgb(0, 200, 200));

            char cost_text[16];
            sprintf(cost_text, "Comprar (%d)", d->cost_to_unlock);

            if (!unlocked)
            {
                Button buy_button = {
                    .x = stats_x - 50,
                    .y = stats_y + 300,
                    .width = 200,
                    .height = 60,
                    .fill_color = &COLOR_GREEN,
                    .text = {.content = cost_text, .color = &COLOR_BLACK, .font = fonts[FONT_H2]},
                    .border = {.border_color = &COLOR_BLACK, .thickness = 2},
                    .on_click = on_buy_defender,
                    .context = d,
                };

                draw_button(&buy_button);
            }
            else if (!equipped)
            {
                Button equip_button = {
                    .x = stats_x - 50,
                    .y = stats_y + 300,
                    .width = 200,
                    .height = 60,
                    .fill_color = &COLOR_BLUE,
                    .text = {.content = "Equipar", .color = &COLOR_WHITE, .font = fonts[FONT_H2]},
                    .border = {.border_color = &COLOR_BLACK, .thickness = 2},
                    .on_click = on_equip_defender,
                    .context = d,
                };

                draw_button(&equip_button);
            }
            else if (get_player_equipped_inventory_size() > 1)
            {
                Button unequip_button = {
                    .x = stats_x - 50,
                    .y = stats_y + 300,
                    .width = 200,
                    .height = 60,
                    .fill_color = &COLOR_RED,
                    .text = {.content = "Desequipar",
                             .color = &COLOR_WHITE,
                             .font = fonts[FONT_H2]},
                    .border = {.border_color = &COLOR_BLACK, .thickness = 2},
                    .on_click = on_unequip_defender,
                    .context = d,
                };

                draw_button(&unequip_button);
            }
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
