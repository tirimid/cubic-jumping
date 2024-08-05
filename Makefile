.PHONY: all

CC := gcc
LD := gcc
CFLAGS := -std=c99 -pedantic -O3 -Iinclude -Imaps $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs)

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%,lib/%.o,$(SRCS))
OUT_BIN := cubic-jumping

all: $(OUT_BIN)

$(OUT_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

lib/%.o: src/% lib
	$(CC) $(CFLAGS) -o $@ -c $<

lib:
	mkdir lib
