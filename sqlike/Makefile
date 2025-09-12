CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Ivendor

SRC = $(filter-out src/main.c, $(wildcard src/*.c))
OBJ = $(SRC:.c=.o)
TARGET = main

TEST_SRC = $(wildcard test/*.c) vendor/unity.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = test_runner

all: $(TARGET)

# Build main program (links with sqlike.o, but also compiles src/main.c)
$(TARGET): $(OBJ) src/main.o
	$(CC) $(CFLAGS) -o $@ $^

# Build & run tests (links only sqlike.o, NOT main.o)
test: $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) src/main.o $(TEST_OBJ) $(TEST_TARGET)
