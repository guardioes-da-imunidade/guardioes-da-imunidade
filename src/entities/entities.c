#include "entities.h"

// TO-DO: Ler de um arquivo JSON para remover a responsabilidade do C saber cada criatura do jogo

Microorganism microorganisms[10];
static int total_entities = 0;

static Defender defenders[] = {
    {
        .base =
            {
                .name = "Glóbulo Branco",
                .description =
                    "Célula de defesa que identifica, engole e destrói microrganismos invasores. "
                    "Representa funções típicas de fagócitos, como neutrófilos e macrófagos.",
                .health = 120,
                .speed = 0.4f,
                .attack = 1,
                .defense = 4,
                .attack_cooldown = 3,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/white_blood_cell.png",
                .image = NULL,
            },
        .cost_to_unlock = 0,
        .cost_to_place = 50,
    },
    {
        .base =
            {
                .name = "Eosinófilo",
                .description =
                    "Leucócito especializado no combate a parasitas, como vermes. Atua também na "
                    "modulação de respostas alérgicas, liberando enzimas e proteínas tóxicas.",
                .health = 80,
                .speed = 0.6f,
                .attack = 3,
                .defense = 3,
                .attack_cooldown = 2.5,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/eosinophil.png",
                .image = NULL,
            },
        .cost_to_unlock = 4,
        .cost_to_place = 75,
    },
    {
        .base =
            {
                .name = "Basófilo",
                .description = "Célula do sistema imunológico envolvida em reações alérgicas e "
                               "inflamatórias. Libera histamina e outras substâncias químicas para "
                               "ajudar na defesa contra patógenos.",
                .health = 100,
                .speed = 0.5f,
                .attack = 5,
                .defense = 5,
                .attack_cooldown = 2,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/basophil.png",
                .image = NULL,
            },
        .cost_to_unlock = 6,
        .cost_to_place = 100,
    },
};

static Enemy enemies[] = {
    {
        .base =
            {
                .name = "Bactéria",
                .description =
                    "Micro-organismo unicelular capaz de se reproduzir rapidamente. Pode causar "
                    "infecções ao liberar toxinas ou invadir tecidos do corpo.",
                .health = 50,
                .speed = 1.2f,
                .attack = 5,
                .defense = 1,
                .attack_cooldown = 1.0f,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/enemies/bacterium.png",
                .image = NULL,
            },
        .is_boss = false,
    },
    {
        .base =
            {
                .name = "Parasita",
                .description =
                    "Organismo que depende de um hospedeiro para sobreviver. Alguns parasitas "
                    "invadem células ou tecidos e podem causar danos prolongados.",
                .health = 60,
                .speed = 1.0f,
                .attack = 6,
                .defense = 2,
                .attack_cooldown = 1.2f,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/enemies/parasite.png",
                .image = NULL,
            },
        .is_boss = false,
    },
    {
        .base =
            {
                .name = "Vírus",
                .description = "Agente infeccioso que invade células do corpo para se replicar. "
                               "Pode se espalhar rapidamente e causar doenças graves.",
                .health = 70,
                .speed = 1.5f,
                .attack = 8,
                .defense = 1,
                .attack_cooldown = 0.8f,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/enemies/virus.png",
                .image = NULL,
            },
        .is_boss = false,
    },
};

static int defenders_count = sizeof(defenders) / sizeof(Defender);
static int enemies_count = sizeof(enemies) / sizeof(Enemy);

void load_entities()
{
    int i, entities_index = 0;

    for (i = 0; i < defenders_count; i++)
    {
        defenders[i].base.id = i;
        microorganisms[entities_index].is_defender = true;
        microorganisms[entities_index].entity = &defenders[i].base;

        ALLEGRO_BITMAP* image = al_load_bitmap(defenders[i].base.image_path);
        defenders[i].base.image = image;
        defenders[i].base.image_width = al_get_bitmap_width(image);
        defenders[i].base.image_height = al_get_bitmap_height(image);

        if (!defenders[i].base.image)
        {
            printf("Falha ao carregar imagem: %s\n", defenders[i].base.image_path);
        }

        entities_index++;
    }

    for (i = 0; i < enemies_count; i++)
    {
        defenders[i].base.id = i;
        microorganisms[entities_index].is_defender = false;
        microorganisms[entities_index].entity = &enemies[i].base;

        ALLEGRO_BITMAP* image = al_load_bitmap(enemies[i].base.image_path);
        enemies[i].base.image = image;
        enemies[i].base.image_width = al_get_bitmap_width(image);
        enemies[i].base.image_height = al_get_bitmap_height(image);

        if (!enemies[i].base.image)
        {
            printf("Falha ao carregar imagem: %s\n", enemies[i].base.image_path);
        }

        entities_index++;
    }

    total_entities = entities_index;
}

const Defender* get_defender_by_id(int index)
{
    if (index < 0 || index >= defenders_count)
    {
        return NULL;
    }

    return &defenders[index];
}

int get_immunecell_count() { return defenders_count; }

int get_entities_count() { return total_entities; }
