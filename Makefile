.PHONY: all

CC := gcc
LD := gcc

CFLAGS := \
	-std=c99 \
	-pedantic \
	-O3 \
	-Iinclude \
	-I. \
	-fno-strict-aliasing \
	-Wall

LDFLAGS := \
	-lm \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_mixer

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%,lib/%.o,$(SRCS))
OUT_BIN := cubic-jumping

all: $(OUT_BIN)

$(OUT_BIN): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

lib/%.o: src/% lib
	$(CC) $(CFLAGS) -o $@ -c $<

lib:
	mkdir lib
