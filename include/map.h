#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAP_MAX_NAME_LEN 8

enum map_tile_type
{
	MTT_AIR = 0,
	MTT_GROUND,
	MTT_KILL,
	MTT_BOUNCE,
	MTT_LAUNCH,
	MTT_END_ON,
	MTT_SWITCH_OFF,
	MTT_SWITCH_ON,
	MTT_END_OFF,
	MTT_SLIPPERY,
	MTT_GRIP,
	MTT_WALL,
	
	MTT_END__,
};

struct map_tile
{
	uint8_t type;
};

struct map
{
	struct map_tile *data;
	uint32_t size_x, size_y;
	uint32_t player_spawn_x, player_spawn_y;
	char name[MAP_MAX_NAME_LEN + 1];
};

extern struct map g_map;

// editor / custom map functionality.
int map_create_file(char const *file, char const *name);
int map_load_from_file(char const *file);
void map_grow(uint32_t dx, uint32_t dy);
void map_refit_bounds(void);
int map_write_to_file(char const *file);

// game and base rendering functionality.
uint8_t const *map_tile_color(enum map_tile_type type);
bool map_tile_collision(enum map_tile_type type);
bool map_tile_slippery(enum map_tile_type type);
bool map_tile_climbable(enum map_tile_type type);
void map_draw(void);
void map_draw_outlines(void);
struct map_tile *map_get(uint32_t x, uint32_t y);

#endif
