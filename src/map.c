#include "map.h"

#include <stddef.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "conf.h"
#include "triggers.h"
#include "util.h"
#include "wnd.h"

struct Map g_Map;

static int RdUint8(uint8_t *Out, FILE *Fp);
static int RdUint32(uint32_t *Out, FILE *Fp);
static void WrUint8(FILE *Fp, uint8_t U8);
static void WrUint32(FILE *Fp, uint32_t U32);

int
Map_CreateFile(char const *File, char const *Name)
{
	unsigned NameLen = strlen(Name);
	if (NameLen > MAP_MAX_NAME_LEN)
	{
		LogErr("map: map name is too long (%u > %u)!", NameLen, MAP_MAX_NAME_LEN);
		return 1;
	}
	
	FILE *Fp = fopen(File, "wb");
	if (!Fp)
	{
		LogErr("map: failed to create file: %s!", File);
		return 1;
	}
	
	// write out header for dummy map.
	{
		fprintf(Fp, "//CJ");
		
		char NameBuf[MAP_MAX_NAME_LEN + 1] = {0};
		strncpy(NameBuf, Name, MAP_MAX_NAME_LEN);
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
			WrUint8(Fp, NameBuf[i]);
		
		WrUint32(Fp, 1);
		WrUint32(Fp, 1);
		WrUint32(Fp, 0);
		WrUint32(Fp, 0);
	}
	
	// write out data for dummy map.
	{
		WrUint8(Fp, MTT_GROUND);
	}
	
	// write out trigger data for dummy map.
	{
		WrUint32(Fp, 0);
	}
	
	// write out inclusion target header.
	{
		fprintf(
			Fp,
			"\n#ifndef %s_HFM\n"
			"#define %s_HFM\n"
			"#include <stddef.h>\n"
			"#include \"map.h\"\n"
			"#include \"triggers.h\"\n"
			"#define %s_NTRIGGERS 0\n"
			"static struct MapTile %s_map_data[] =\n"
			"{\n"
			"{1},\n"
			"};\n"
			"static struct Map %s_map =\n"
			"{\n"
			"\t.SizeX = 1,\n"
			"\t.SizeY = 1,\n"
			"\t.Data = %s_data,\n"
			"\t.Name = \"%s\\0\",\n"
			"};\n"
			"static struct Trigger %s_triggers[] =\n"
			"{\n"
			"{0.0f,0.0f,0.0f,0.0f,0,0,0}\n"
			"};\n"
			"#endif\n",
			Name,
			Name,
			Name,
			Name,
			Name,
			Name,
			Name,
			Name
		);
	}
	
	fclose(Fp);
	
	return 0;
}

int
Map_LoadFromFile(char const *File)
{
	FILE *Fp = fopen(File, "rb");
	if (!Fp)
	{
		LogErr("map: failed to open file: %s!", File);
		return 1;
	}
	
	// read map header data.
	{
		if (fgetc(Fp) != '/'
			|| fgetc(Fp) != '/'
			|| fgetc(Fp) != 'C'
			|| fgetc(Fp) != 'J')
		{
			LogErr("map: file contains invalid header: %s!", File);
			return 1;
		}
		
		memset(g_Map.Name, 0, sizeof(g_Map.Name));
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
		{
			uint8_t Ch;
			if (RdUint8(&Ch, Fp))
				return 1;
			g_Map.Name[i] = Ch;
		}
		
		if (RdUint32(&g_Map.SizeX, Fp)
			|| RdUint32(&g_Map.SizeY, Fp)
			|| RdUint32(&g_Map.PlayerSpawnX, Fp)
			|| RdUint32(&g_Map.PlayerSpawnY, Fp))
		{
			return 1;
		}
	}
	
	// read main map data.
	{
		g_Map.Data = malloc(sizeof(struct MapTile) * g_Map.SizeX * g_Map.SizeY);
		for (size_t i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
		{
			uint8_t Type;
			if (RdUint8(&Type, Fp))
				return 1;
			
			g_Map.Data[i] = (struct MapTile)
			{
				.Type = Type
			};
		}
	}
	
	// read trigger data.
	{
		uint32_t TriggerCnt;
		if (RdUint32(&TriggerCnt, Fp))
			return 1;
		
		g_TriggerCnt = 0;
		for (uint32_t i = 0; i < TriggerCnt; ++i)
		{
			struct Trigger NewTrigger;
			if (RdUint32((uint32_t *)&NewTrigger.PosX, Fp)
				|| RdUint32((uint32_t *)&NewTrigger.PosY, Fp)
				|| RdUint32((uint32_t *)&NewTrigger.SizeX, Fp)
				|| RdUint32((uint32_t *)&NewTrigger.SizeY, Fp)
				|| RdUint32((uint32_t *)&NewTrigger.Arg, Fp)
				|| RdUint8((uint8_t *)&NewTrigger.SingleUse, Fp)
				|| RdUint8(&NewTrigger.Type, Fp))
			{
				LogErr("map: failed to read one or more triggers from file: %s!", File);
				return 1;
			}
			
			Triggers_AddTrigger(&NewTrigger);
		}
	}
	
	fclose(Fp);
	
	return 0;
}

void
Map_Grow(uint32_t Dx, uint32_t Dy)
{
	uint32_t OldSizeX = g_Map.SizeX, OldSizeY = g_Map.SizeY;
	g_Map.SizeX += Dx;
	g_Map.SizeY += Dy;
	
	g_Map.Data = realloc(g_Map.Data, sizeof(struct MapTile) * g_Map.SizeX * g_Map.SizeY);
	
	// create new air cells (horizontal).
	{
		size_t MvLen = OldSizeX * (OldSizeY - 1);
		size_t MvIdx = OldSizeX;
		while (MvLen > 0)
		{
			memmove(
				&g_Map.Data[MvIdx + Dx],
				&g_Map.Data[MvIdx],
				sizeof(struct MapTile) * MvLen
			);
			memset(&g_Map.Data[MvIdx], 0, sizeof(struct MapTile) * Dx);
			MvLen -= OldSizeX;
			MvIdx += g_Map.SizeX;
		}
		
		// need to zero new tiles on last row to prevent phantom tiles from
		// randomly appearing upon map grow.
		memset(&g_Map.Data[MvIdx], 0, sizeof(struct MapTile) * Dx);
	}
	
	// create new air cells (vertical).
	{
		memset(
			&g_Map.Data[g_Map.SizeX * OldSizeY],
			0,
			sizeof(struct MapTile) * g_Map.SizeX * Dy
		);
	}
}

void
Map_RefitBounds(void)
{
	// determine minimum bounds of map.
	uint32_t FarX = 0, FarY = 0;
	for (uint32_t x = 0; x < g_Map.SizeX; ++x)
	{
		for (uint32_t y = 0; y < g_Map.SizeY; ++y)
		{
			struct MapTile *Tile = Map_Get(x, y);
			if (Tile->Type == MTT_AIR)
				continue;
			
			FarY = MAX(FarY, y);
			FarX = MAX(FarX, x);
		}
	}
	
	// if needed, shrink horizontally and move cell memory.
	if (g_Map.SizeX > FarX + 1)
	{
		size_t Dx = g_Map.SizeX - FarX - 1;
		size_t MvIdx = g_Map.SizeX;
		size_t MvLen = g_Map.SizeX * (g_Map.SizeY - 1);
		
		while (MvLen > 0)
		{
			memmove(
				&g_Map.Data[MvIdx - Dx],
				&g_Map.Data[MvIdx],
				sizeof(struct MapTile) * MvLen
			);
			
			MvLen -= g_Map.SizeX;
			MvIdx += FarX + 1;
		}
	}
	
	// resize map to minimum possible size.
	{
		g_Map.SizeX = FarX + 1;
		g_Map.SizeY = FarY + 1;
	}
}

int
Map_WriteToFile(char const *File)
{
	FILE *Fp = fopen(File, "wb");
	if (!Fp)
	{
		LogErr("map: failed to open file: %s!", File);
		return 1;
	}
	
	// write out map data header.
	{
		fprintf(Fp, "//CJ");
		
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
			WrUint8(Fp, g_Map.Name[i]);
		
		WrUint32(Fp, g_Map.SizeX);
		WrUint32(Fp, g_Map.SizeY);
		WrUint32(Fp, g_Map.PlayerSpawnX);
		WrUint32(Fp, g_Map.PlayerSpawnY);
		
		for (size_t i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
			WrUint8(Fp, g_Map.Data[i].Type);
	}
	
	// write out trigger data.
	{
		WrUint32(Fp, g_TriggerCnt);
		
		for (size_t i = 0; i < g_TriggerCnt; ++i)
		{
			struct Trigger const *Trigger = &g_Triggers[i];
			
			WrUint32(Fp, *(uint32_t *)&Trigger->PosX);
			WrUint32(Fp, *(uint32_t *)&Trigger->PosY);
			WrUint32(Fp, *(uint32_t *)&Trigger->SizeX);
			WrUint32(Fp, *(uint32_t *)&Trigger->SizeY);
			WrUint32(Fp, Trigger->Arg);
			WrUint8(Fp, Trigger->SingleUse);
			WrUint8(Fp, Trigger->Type);
		}
	}
	
	fprintf(Fp, "\n");
	
	// write out inclusion target header text.
	{
		fprintf(
			Fp,
			"#ifndef %s_HFM\n"
			"#define %s_HFM\n"
			"#include <stddef.h>\n"
			"#include \"map.h\"\n"
			"#include \"triggers.h\"\n"
			"#define %s_NTRIGGERS %lu\n"
			"static struct MapTile %s_map_data[] =\n"
			"{\n",
			g_Map.Name,
			g_Map.Name,
			g_Map.Name,
			g_TriggerCnt,
			g_Map.Name
		);
		
		for (size_t i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
			fprintf(Fp, "{%u},", g_Map.Data[i].Type);
		
		fprintf(
			Fp,
			"\n};\n"
			"static struct Map %s_map =\n"
			"{\n"
			"\t.SizeX = %u,\n"
			"\t.SizeY = %u,\n"
			"\t.Data = %s_map_data,\n"
			"\t.PlayerSpawnX = %u,\n"
			"\t.PlayerSpawnY = %u,\n"
			"\t.Name = \"%s\\0\",\n"
			"};\n"
			"static struct Trigger %s_triggers[] =\n"
			"{\n",
			g_Map.Name,
			g_Map.SizeX,
			g_Map.SizeY,
			g_Map.Name,
			g_Map.PlayerSpawnX,
			g_Map.PlayerSpawnY,
			g_Map.Name,
			g_Map.Name
		);
		
		for (size_t i = 0; i < g_TriggerCnt; ++i)
		{
			fprintf(
				Fp,
				"{%ff,%ff,%ff,%ff,%u,%u,%u},",
				g_Triggers[i].PosX,
				g_Triggers[i].PosY,
				g_Triggers[i].SizeX,
				g_Triggers[i].SizeY,
				g_Triggers[i].Arg,
				g_Triggers[i].SingleUse,
				g_Triggers[i].Type
			);
		}
		
		fprintf(
			Fp,
			"{0.0f,0.0f,0.0f,0.0f,0,0,0}\n" // dummy trigger.
			"};\n"
			"#endif\n"
		);
	}
	
	fclose(Fp);
	
	return 0;
}

uint8_t const *
Map_TileColor(enum MapTileType Type)
{
	static uint8_t Colors[MTT_END__][3] =
	{
		CONF_COLOR_BG,
		CONF_COLOR_GROUND,
		CONF_COLOR_KILL,
		CONF_COLOR_BOUNCE,
		CONF_COLOR_LAUNCH,
		CONF_COLOR_END_ON,
		CONF_COLOR_SWITCH_OFF,
		CONF_COLOR_SWITCH_ON,
		CONF_COLOR_END_OFF,
		CONF_COLOR_SLIPPERY,
		CONF_COLOR_GRIP,
		CONF_COLOR_WALL
	};
	
	return Colors[Type];
}

bool
Map_TileCollision(enum MapTileType Type)
{
	static bool Collision[MTT_END__] =
	{
		false, // air.
		true, // ground.
		true, // kill.
		true, // bounce.
		true, // launch.
		true, // end on.
		true, // switch off.
		true, // switch on.
		true, // end off.
		true, // slippery.
		true, // grip.
		false // wall.
	};
	
	return Collision[Type];
}

bool
Map_TileSlippery(enum MapTileType Type)
{
	static bool Slippery[MTT_END__] =
	{
		false, // air.
		false, // ground.
		false, // kill.
		false, // bounce.
		false, // launch.
		false, // end on.
		false, // switch off.
		false, // switch on.
		false, // end off.
		true, // slippery.
		false // grip.
	};
	
	return Slippery[Type];
}

bool
Map_TileClimbable(enum MapTileType Type)
{
	static bool Climbable[MTT_END__] =
	{
		false, // air.
		false, // ground.
		false, // kill.
		false, // bounce.
		false, // launch.
		false, // end on.
		false, // switch off.
		false, // switch on.
		false, // end off.
		false, // slippery.
		true, // grip.
		false // wall.
	};
	
	return Climbable[Type];
}

void
Map_Draw(void)
{
	for (uint32_t x = 0; x < g_Map.SizeX; ++x)
	{
		for (uint32_t y = 0; y < g_Map.SizeY; ++y)
		{
			struct MapTile *Tile = Map_Get(x, y);
			if (Tile->Type == MTT_AIR)
				continue;
			
			uint8_t const *Col = Map_TileColor(Tile->Type);
			SDL_SetRenderDrawColor(g_Rend, Col[0], Col[1], Col[2], 255);
			RelativeDrawRect(x, y, 1.0f, 1.0f);
		}
	}
}

void
Map_DrawOutlines(void)
{
	static uint8_t Co[] = CONF_COLOR_OUTLINE;
	
	SDL_SetRenderDrawColor(g_Rend, Co[0], Co[1], Co[2], 255);
	for (uint32_t x = 0; x < g_Map.SizeX; ++x)
	{
		for (uint32_t y = 0; y < g_Map.SizeY; ++y)
			RelativeDrawHollowRect(x, y, 1.0f, 1.0f);
	}
}

struct MapTile *
Map_Get(uint32_t x, uint32_t y)
{
	return &g_Map.Data[g_Map.SizeX * y + x];
}

static int
RdUint8(uint8_t *Out, FILE *Fp)
{
	int High = fgetc(Fp);
	if (High == EOF)
	{
		LogErr("map: failed to read U8 higher-half!");
		return 1;
	}
	
	int Low = fgetc(Fp);
	if (Low == EOF)
	{
		LogErr("map: failed to read U8 lower-half!");
		return 1;
	}
	
	static uint8_t ValLookup[] =
	{
		['0'] = 0,
		['1'] = 1,
		['2'] = 2,
		['3'] = 3,
		['4'] = 4,
		['5'] = 5,
		['6'] = 6,
		['7'] = 7,
		['8'] = 8,
		['9'] = 9,
		['a'] = 10,
		['b'] = 11,
		['c'] = 12,
		['d'] = 13,
		['e'] = 14,
		['f'] = 15,
	};
	
	*Out = (uint8_t)ValLookup[Low];
	*Out |= (uint8_t)ValLookup[High] << 4;
	return 0;
}

static int
RdUint32(uint32_t *Out, FILE *Fp)
{
	uint8_t B0, B1, B2, B3;
	if (RdUint8(&B3, Fp)
		|| RdUint8(&B2, Fp)
		|| RdUint8(&B1, Fp)
		|| RdUint8(&B0, Fp))
	{
		return 1;
	}
	
	*Out = (uint32_t)B0;
	*Out |= (uint32_t)B1 << 8;
	*Out |= (uint32_t)B2 << 16;
	*Out |= (uint32_t)B3 << 24;
	return 0;
}

static void
WrUint8(FILE *Fp, uint8_t U8)
{
	static uint8_t HexLookup[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
	};
	
	fprintf(Fp, "%c%c", HexLookup[U8 >> 4], HexLookup[U8 & 0xf]);
}

static void
WrUint32(FILE *Fp, uint32_t U32)
{
	uint8_t B0 = U32 & 0xff;
	uint8_t B1 = (U32 & 0xff00) >> 8;
	uint8_t B2 = (U32 & 0xff0000) >> 16;
	uint8_t B3 = (U32 & 0xff000000) >> 24;
	
	WrUint8(Fp, B3);
	WrUint8(Fp, B2);
	WrUint8(Fp, B1);
	WrUint8(Fp, B0);
}
