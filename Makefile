TARGET = game

SRC = main.c \
      src/core/init.c \
      src/core/event.c \
      src/systems/resource.c

CC = gcc

LIBS = -lallegro -lallegro_dialog -lallegro_image -lallegro_font -lallegro_ttf

# Cria a pasta ./build se ela não existir
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Regra padrão (simplesmente "make" no terminal)
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

# Limpa arquivos gerados
clean:
	rm -f $(BUILD_DIR)/$(TARGET)
