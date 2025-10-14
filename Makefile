TARGET = game

# Todos os arquivos .c vão ser inclusos na build
SRC = $(shell find src -name "*.c") main.c

CC = gcc

LIBS = -lallegro -lallegro_dialog -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives

# -Wall - All warnings
# -Wextra - Mais avisos que o -Wall não cobre
# -std=c11 - Qual padrão da linguagem C usar, esse é o moderno
# -Isrc - Diz para o compilador onde encontrar os arquivos headers
# -g - Inclui informações de depuração no binário
# -O0 - Diz para o compilador não otimizar o código, útil para deixar mais literal ao que escrevemos
# Flags para build normal
CFLAGS = -Wall -Wextra -std=c11 -Isrc
# Flags para debug
DEBUG_FLAGS = -Wall -Wextra -std=c11 -Isrc -g -O0

# Cria a pasta ./build se ela não existir
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Build normal (simplesmente "make" no terminal)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

# Build para debug
debug: $(SRC)
	$(CC) $(DEBUG_FLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

# Limpa arquivos gerados
clean:
	rm -f $(BUILD_DIR)/$(TARGET)
