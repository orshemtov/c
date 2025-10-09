CC = clang
CFLAGS = \
	-Wall \
	-Werror \
	-Wextra \
	-std=c17 \
	-g \
	-Iinclude \
	-Ivendor \
	-lreadline

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
VENDOR_DIR = vendor

# Sources
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# Library sources (exclude main.c for tests)
LIB_SRC = $(filter-out $(SRC_DIR)/main.c, $(SRC))
LIB_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRC))

# Test sources
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRC))

# Unity framework
UNITY_SRC = $(VENDOR_DIR)/unity.c
UNITY_OBJ = $(BUILD_DIR)/unity.o

# Targets
TARGET = $(BUILD_DIR)/minidb
TEST_TARGET = $(BUILD_DIR)/test_runner

.PHONY: all clean test run

all: $(TARGET)

# Main binary
$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Tests - run all test files
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(LIB_OBJ) $(TEST_OBJ) $(UNITY_OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Object file rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/unity.o: $(VENDOR_DIR)/unity.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build dir exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

format:
	clang-format -i $(SRC) $(TEST_SRC) include/*.h
