#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#include <SDL.h>

typedef enum map_tile_type
{
	MTT_AIR = 0,
	MTT_GROUND,
} map_tile_type_t;

typedef struct map_tile
{
	uint8_t type;
} map_tile_t;

typedef struct map
{
	map_tile_t *data;
	unsigned size_x, size_y;
} map_t;

extern map_t g_map;

float const *map_tile_color(map_tile_type_t type);
void map_draw(SDL_Renderer *rend);
map_tile_t *map_get(unsigned x, unsigned y);

#endif
