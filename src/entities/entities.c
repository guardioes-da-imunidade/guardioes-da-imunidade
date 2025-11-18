#include "entities.h"

// TO-DO: Ler de um arquivo JSON para remover a responsabilidade do C saber cada criatura do jogo

Microorganism microorganisms[10];
int total_microorganisms = 0;

static ImmuneCell defenders[] = {
    {
        .base =
            {
                .name = "Basófilo",
                .description = "Célula do sistema imunológico envolvida em reações alérgicas e "
                               "inflamatórias. Libera histamina e outras substâncias químicas para "
                               "ajudar na defesa contra patógenos.",
                .health = 100,
                .speed = 0.5f,
                .attack = 0,
                .defense = 5,
                .attack_cooldown = 0,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/basophil.png",
                .image = NULL,
            },
        .cost_to_place = 10,
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
                .attack = 5,
                .defense = 3,
                .attack_cooldown = 1.0f,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/eosinophil.png",
                .image = NULL,
            },
        .cost_to_place = 12,
    },
    {
        .base =
            {
                .name = "Glóbulo Branco",
                .description =
                    "Célula de defesa que identifica, engole e destrói microrganismos invasores. "
                    "Representa funções típicas de fagócitos, como neutrófilos e macrófagos.",
                .health = 120,
                .speed = 0.4f,
                .attack = 10,
                .defense = 4,
                .attack_cooldown = 1.5f,
                .active = false,
                .row = 0,
                .col = 0,
                .x = 0,
                .y = 0,
                .image_path = "assets/images/defenders/white_blood_cell.png",
                .image = NULL,
            },
        .cost_to_place = 15,
    },
};

static Pathogen enemies[] = {
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

static int defenders_count = sizeof(defenders) / sizeof(ImmuneCell);
static int enemies_count = sizeof(enemies) / sizeof(Pathogen);

void load_microorganisms()
{
    int i, microorganisms_index = 0;

    for (i = 0; i < defenders_count; i++)
    {
        microorganisms[microorganisms_index].is_defender = true;
        microorganisms[microorganisms_index].entity = &defenders[i].base;

        defenders[i].base.image = al_load_bitmap(defenders[i].base.image_path);

        if (!defenders[i].base.image)
        {
            printf("Falha ao carregar imagem: %s\n", defenders[i].base.image_path);
        }

        microorganisms_index++;
    }

    for (i = 0; i < enemies_count; i++)
    {
        microorganisms[microorganisms_index].is_defender = false;
        microorganisms[microorganisms_index].entity = &enemies[i].base;

        enemies[i].base.image = al_load_bitmap(enemies[i].base.image_path);

        if (!enemies[i].base.image)
        {
            printf("Falha ao carregar imagem: %s\n", enemies[i].base.image_path);
        }

        microorganisms_index++;
    }

    total_microorganisms = microorganisms_index;
}
