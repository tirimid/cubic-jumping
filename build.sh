#!/bin/bash

if [ "$#" -ne 1 ]
then
	echo "usage: $0 jumbo/data/all" >&2
	exit 1
fi

CC=gcc
LD=gcc
CFLAGS="-I. -Iinclude -Wall -O3 -std=c99 -pedantic $(pkg-config --cflags sdl2 SDL2_image SDL2_mixer)"
LDFLAGS="$(pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer) -lm"

mkdir -p lib

if [ "$1" == "all" ] || [ "$1" == "data" ]
then
	echo "build > data"
	$CC $CFLAGS -o lib/build_data.o -c src/build_data.c
	if [ $? -ne 0 ]
	then
		echo "err: build failed on data!" >&2
		exit 1
	fi
fi

if [ "$1" == "all" ] || [ "$1" == "jumbo" ]
then
	echo "build > jumbo"
	$CC $CFLAGS -o lib/build_jumbo.o -c src/build_jumbo.c
	if [ $? -ne 0 ]
	then
		echo "err: build failed on jumbo!" >&2
		exit 1
	fi
fi

echo "build > link"
$LD -o cubic-jumping lib/build_data.o lib/build_jumbo.o $LDFLAGS
if [ $? -ne 0 ]
then
	echo "err: build failed on link!" >&2
	exit 1
fi
