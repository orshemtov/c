CC = gcc
CFLAGS = -Wall -Wextra -std=c2x 
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)