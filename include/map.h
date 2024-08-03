#ifndef MAP_H
#define MAP_H

#include <stdint.h>

#include <SDL.h>

typedef enum map_tile_type
{
	MTT_AIR = 0,
	MTT_GROUND,
	
	MTT_END__,
} map_tile_type_t;

typedef struct map_tile
{
	uint8_t type;
} map_tile_t;

typedef struct map
{
	map_tile_t *data;
	uint32_t size_x, size_y;
	uint32_t player_spawn_x, player_spawn_y;
} map_t;

extern map_t g_map;

int map_load_from_file(char const *file);
void map_grow(uint32_t dx, uint32_t dy);
int map_write_to_file(char const *file, char const *name);
float const *map_tile_color(map_tile_type_t type);
void map_draw(SDL_Renderer *rend);
map_tile_t *map_get(uint32_t x, uint32_t y);

#endif
