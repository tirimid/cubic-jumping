#include "map.h"

#include <stdio.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "triggers.h"
#include "wnd.h"

struct Map g_Map;

u8 Map_TileColor[MTT_END__][3] =
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

bool Map_TileCollision[MTT_END__] =
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

bool Map_TileSlippery[MTT_END__] =
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
	false, // grip.
	false // wall.
};

bool Map_TileClimbable[MTT_END__] =
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

static i32 RdUint8(u8 *Out, FILE *Fp);
static i32 RdUint32(u32 *Out, FILE *Fp);
static void WrUint8(FILE *Fp, u8 U8);
static void WrUint32(FILE *Fp, u32 U32);

i32
Map_CreateFile(char const *File, char const *Name)
{
	u32 NameLen = strlen(Name);
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
		for (usize i = 0; i < MAP_MAX_NAME_LEN; ++i)
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

i32
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
		for (usize i = 0; i < MAP_MAX_NAME_LEN; ++i)
		{
			u8 Ch;
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
		for (usize i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
		{
			u8 Type;
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
		u32 TriggerCnt;
		if (RdUint32(&TriggerCnt, Fp))
			return 1;
		
		g_TriggerCnt = 0;
		for (u32 i = 0; i < TriggerCnt; ++i)
		{
			struct Trigger NewTrigger;
			if (RdUint32((u32 *)&NewTrigger.PosX, Fp)
				|| RdUint32((u32 *)&NewTrigger.PosY, Fp)
				|| RdUint32((u32 *)&NewTrigger.SizeX, Fp)
				|| RdUint32((u32 *)&NewTrigger.SizeY, Fp)
				|| RdUint32((u32 *)&NewTrigger.Arg, Fp)
				|| RdUint8((u8 *)&NewTrigger.SingleUse, Fp)
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
Map_Grow(u32 Dx, u32 Dy)
{
	u32 OldSizeX = g_Map.SizeX, OldSizeY = g_Map.SizeY;
	g_Map.SizeX += Dx;
	g_Map.SizeY += Dy;
	
	g_Map.Data = realloc(g_Map.Data, sizeof(struct MapTile) * g_Map.SizeX * g_Map.SizeY);
	
	// create new air cells (horizontal).
	{
		usize MvLen = OldSizeX * (OldSizeY - 1);
		usize MvIdx = OldSizeX;
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
	u32 FarX = 0, FarY = 0;
	for (u32 x = 0; x < g_Map.SizeX; ++x)
	{
		for (u32 y = 0; y < g_Map.SizeY; ++y)
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
		usize Dx = g_Map.SizeX - FarX - 1;
		usize MvIdx = g_Map.SizeX;
		usize MvLen = g_Map.SizeX * (g_Map.SizeY - 1);
		
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

i32
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
		
		for (usize i = 0; i < MAP_MAX_NAME_LEN; ++i)
			WrUint8(Fp, g_Map.Name[i]);
		
		WrUint32(Fp, g_Map.SizeX);
		WrUint32(Fp, g_Map.SizeY);
		WrUint32(Fp, g_Map.PlayerSpawnX);
		WrUint32(Fp, g_Map.PlayerSpawnY);
		
		for (usize i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
			WrUint8(Fp, g_Map.Data[i].Type);
	}
	
	// write out trigger data.
	{
		WrUint32(Fp, g_TriggerCnt);
		
		for (usize i = 0; i < g_TriggerCnt; ++i)
		{
			struct Trigger const *Trigger = &g_Triggers[i];
			
			WrUint32(Fp, *(u32 *)&Trigger->PosX);
			WrUint32(Fp, *(u32 *)&Trigger->PosY);
			WrUint32(Fp, *(u32 *)&Trigger->SizeX);
			WrUint32(Fp, *(u32 *)&Trigger->SizeY);
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
		
		for (usize i = 0; i < g_Map.SizeX * g_Map.SizeY; ++i)
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
		
		for (usize i = 0; i < g_TriggerCnt; ++i)
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

void
Map_Draw(void)
{
	f32 MinX, MinY;
	ScreenToGameCoord(&MinX, &MinY, 0, 0);
	MinX = MAX(0.0f, MinX);
	MinY = MAX(0.0f, MinY);
	
	f32 MaxX, MaxY;
	ScreenToGameCoord(&MaxX, &MaxY, CONF_WND_WIDTH, CONF_WND_HEIGHT);
	MaxX = MIN((u32)(MaxX + 1.0f), g_Map.SizeX);
	MaxY = MIN((u32)(MaxY + 1.0f), g_Map.SizeY);
	
	for (u32 x = MinX; x < (u32)MaxX; ++x)
	{
		for (u32 y = MinY; y < (u32)MaxY; ++y)
		{
			struct MapTile *Tile = Map_Get(x, y);
			if (Tile->Type == MTT_AIR)
				continue;
			
			u8 const *Col = &Map_TileColor[Tile->Type][0];
			SDL_SetRenderDrawColor(g_Rend, Col[0], Col[1], Col[2], 255);
			RelativeDrawRect(x, y, 1.0f, 1.0f);
		}
	}
}

void
Map_DrawOutlines(void)
{
	static u8 Co[] = CONF_COLOR_OUTLINE;
	
	SDL_SetRenderDrawColor(g_Rend, Co[0], Co[1], Co[2], 255);
	for (u32 x = 0; x < g_Map.SizeX; ++x)
	{
		for (u32 y = 0; y < g_Map.SizeY; ++y)
			RelativeDrawHollowRect(x, y, 1.0f, 1.0f);
	}
}

struct MapTile *
Map_Get(u32 x, u32 y)
{
	return &g_Map.Data[g_Map.SizeX * y + x];
}

static i32
RdUint8(u8 *Out, FILE *Fp)
{
	i32 High = fgetc(Fp);
	if (High == EOF)
	{
		LogErr("map: failed to read U8 higher-half!");
		return 1;
	}
	
	i32 Low = fgetc(Fp);
	if (Low == EOF)
	{
		LogErr("map: failed to read U8 lower-half!");
		return 1;
	}
	
	static u8 ValLookup[] =
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
	
	*Out = (u8)ValLookup[Low];
	*Out |= (u8)ValLookup[High] << 4;
	return 0;
}

static i32
RdUint32(u32 *Out, FILE *Fp)
{
	u8 B0, B1, B2, B3;
	if (RdUint8(&B3, Fp)
		|| RdUint8(&B2, Fp)
		|| RdUint8(&B1, Fp)
		|| RdUint8(&B0, Fp))
	{
		return 1;
	}
	
	*Out = (u32)B0;
	*Out |= (u32)B1 << 8;
	*Out |= (u32)B2 << 16;
	*Out |= (u32)B3 << 24;
	return 0;
}

static void
WrUint8(FILE *Fp, u8 U8)
{
	static u8 HexLookup[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
	};
	
	fprintf(Fp, "%c%c", HexLookup[U8 >> 4], HexLookup[U8 & 0xf]);
}

static void
WrUint32(FILE *Fp, u32 U32)
{
	u8 B0 = U32 & 0xff;
	u8 B1 = (U32 & 0xff00) >> 8;
	u8 B2 = (U32 & 0xff0000) >> 16;
	u8 B3 = (U32 & 0xff000000) >> 24;
	
	WrUint8(Fp, B3);
	WrUint8(Fp, B2);
	WrUint8(Fp, B1);
	WrUint8(Fp, B0);
}
