TARGET = game

SRC = main.c

CC = gcc

LIBS = -lallegro

# Cria a pasta ./build se ela não existir
BUILD_DIR = build
$(shell mkdir -p $(BUILD_DIR))

# Regra padrão (simplesmente "make" no terminal)
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

# Limpa arquivos gerados
clean:
	rm -f $(BUILD_DIR)/$(TARGET)
