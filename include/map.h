#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>

#define MAP_MAX_NAME_LEN 8

enum MapTileType
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
	
	MTT_END__
};

struct MapTile
{
	uint8_t Type;
};

struct Map
{
	struct MapTile *Data;
	uint32_t SizeX, SizeY;
	uint32_t PlayerSpawnX, PlayerSpawnY;
	char Name[MAP_MAX_NAME_LEN + 1];
};

extern struct Map g_Map;

// editor / custom map functionality.
int Map_CreateFile(char const *File, char const *Name);
int Map_LoadFromFile(char const *File);
void Map_Grow(uint32_t Dx, uint32_t Dy);
void Map_RefitBounds(void);
int Map_WriteToFile(char const *File);

// game and base rendering functionality.
uint8_t const *Map_TileColor(enum MapTileType Type);
bool Map_TileCollision(enum MapTileType Type);
bool Map_TileSlippery(enum MapTileType Type);
bool Map_TileClimbable(enum MapTileType Type);
void Map_Draw(void);
void Map_DrawOutlines(void);
struct MapTile *Map_Get(uint32_t x, uint32_t y);

#endif
