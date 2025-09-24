BUILD_DIR = build

CC = gcc
CFLAGS = -Wall -Wextra -std=c17
SRC = main.c
OBJ = $(BUILD_DIR)/main.o
TARGET = main

all: $(BUILD_DIR)/$(TARGET)
	@./$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
