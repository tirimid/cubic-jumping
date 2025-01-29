#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#include "conf.h"
#include "util.h"

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
	MTT_BEAM,
	MTT_SWAP_ON,
	MTT_SWAP_OFF,
	
	MTT_END__
};

struct MapTile
{
	u8 Type;
};

struct Map
{
	struct MapTile *Data;
	u32 SizeX, SizeY;
	u32 PlayerSpawnX, PlayerSpawnY;
	char Name[MAP_MAX_NAME_LEN + 1];
};

extern struct Map g_Map;

// tables.
extern u8 Map_TileColor[MTT_END__][3];
extern bool Map_TileCollision[MTT_END__];
extern bool Map_TileSlippery[MTT_END__];
extern bool Map_TileClimbable[MTT_END__];

// editor / custom map functionality.
i32 Map_CreateFile(char const *File, char const *Name);
i32 Map_LoadFromFile(char const *File);
void Map_Grow(u32 Dx, u32 Dy);
void Map_RefitBounds(void);
i32 Map_WriteToFile(char const *File);

// game and base rendering functionality.
void Map_Update(void);
void Map_Draw(void);
void Map_DrawOutlines(void);
struct MapTile *Map_Get(u32 x, u32 y);

#endif
