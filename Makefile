TARGET = game

SRC = main.c \
    src/core/init.c \
    src/core/game.c \
	src/core/palette.c \
	src/core/typography.c \
	src/core/input.c \
	src/core/ui/button.c \
	src/core/ui/vaccines.c \
	\
    src/screens/base/menu.c \
    src/screens/base/config.c \
	src/screens/bestiary/bestiary.c \
    src/screens/game_screen/game_screen.c \
	src/screens/lobby_screen/lobby_screen.c \
	src/screens/endless_mode_screen/endless_mode_screen.c \
	src/screens/stage_screen/stage_screen.c \
	\
	src/systems/sound_effect.c \
	src/systems/music.c \
	src/systems/global_audio.c \
    src/systems/resource.c \
	\
	src/entities/entities.c \
	src/entities/player/player.c

CC = gcc

LIBS = -lallegro -lallegro_dialog -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_audio -lallegro_acodec

# Cria a pasta ./build se ela não existir
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Regra padrão (simplesmente "make" no terminal)
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS) -lm

# Limpa arquivos gerados
clean:
	rm -f $(BUILD_DIR)/$(TARGET)
