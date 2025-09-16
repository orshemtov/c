# Makefile

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS  =
LIBS     = -lcurl -lcjson
SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:.c=.o)
TARGET   = main

# If Homebrew is present, add its include/lib paths (macOS)
HB_PREFIX := $(shell brew --prefix 2>/dev/null)
ifneq ($(HB_PREFIX),)
  CFLAGS  += -I$(HB_PREFIX)/include
  LDFLAGS += -L$(HB_PREFIX)/lib
endif

# Optional: try pkg-config (helps on Linux)
# (won't fail if not installed)
PKG_LIBS  := $(shell pkg-config --libs --silence-errors libcurl libcjson)
PKG_CFLAGS:= $(shell pkg-config --cflags --silence-errors libcurl libcjson)
ifneq ($(PKG_LIBS),)
  LIBS    := $(PKG_LIBS)
endif
ifneq ($(PKG_CFLAGS),)
  CFLAGS  += $(PKG_CFLAGS)
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

# Generic compile rule: any .c -> .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
