BUILD_DIR = build

CC = gcc
CFLAGS = -Wall -Wextra -std=c17

# Automatically find all .c files in current directory
SRC = $(wildcard *.c)
# Generate object file names from source files
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
TARGET = main

all: $(BUILD_DIR)/$(TARGET)
	@./$(BUILD_DIR)/$(TARGET)

# Pattern rule for compiling .c files to .o files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
