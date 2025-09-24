BUILD := build

CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c17 -Iinclude -Ivendor
LDFLAGS = 
LDLIBS = 

SRC = $(filter-out src/main.c, $(wildcard src/*.c))
OBJ = $(patsubst src/%.c, $(BUILD)/src/%.o, $(SRC))
MAIN_OBJ = $(BUILD)/src/main.o
TARGET = $(BUILD)/main

TEST_SRC = $(wildcard tests/*.c)
TEST_OBJ = $(patsubst tests/%.c, $(BUILD)/tests/%.o, $(TEST_SRC))
TEST_OBJ += $(BUILD)/vendor/unity.o
TEST_TARGET = $(BUILD)/test_runner

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ) $(MAIN_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ 

test: $(OBJ) $(TEST_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^
	./$(TEST_TARGET)

# Create build directories
$(BUILD):
	mkdir -p $(BUILD)/src $(BUILD)/tests $(BUILD)/vendor

# Pattern rules for compiling
$(BUILD)/src/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/tests/%.o: tests/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/vendor/%.o: vendor/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)