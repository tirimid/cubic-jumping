#include "map_list.h"

#include <stdlib.h>

#include "cam.h"
#include "game.h"
#include "map.h"
#include "menus.h"
#include "player.h"
#include "save.h"
#include "sequences.h"
#include "text_list.h"
#include "triggers.h"
#include "vfx.h"

// compiled map data.

// chapter 0.
#include "maps/c0e0.hfm"
#include "maps/c0e1.hfm"
#include "maps/c0e2.hfm"
#include "maps/c0e3.hfm"
#include "maps/c0e4.hfm"
#include "maps/c0e5.hfm"
#include "maps/c0e6.hfm"
#include "maps/c0e7.hfm"
#include "maps/c0e8.hfm"
#include "maps/c0e9.hfm"
#include "maps/c0e10.hfm"
#include "maps/c0e11.hfm"
#include "maps/c0e12.hfm"
#include "maps/c0e13.hfm"
#include "maps/c0e14.hfm"
#include "maps/c0e15.hfm"
#include "maps/c0e16.hfm"
#include "maps/c0e17.hfm"
#include "maps/c0e18.hfm"
#include "maps/c0e19.hfm"
#include "maps/c0e20.hfm"
#include "maps/c0e21.hfm"
#include "maps/c0e22.hfm"
#include "maps/c0e23.hfm"
#include "maps/c0e24.hfm"

#define INCLUDE_MAP(Name) \
	{ \
		.Map = &Name##_map, \
		.Triggers = Name##_triggers, \
		.TriggerCnt = Name##_NTRIGGERS, \
		.Decals = Name##_decals, \
		.DecalCnt = Name##_NDECALS \
	}

struct Item
{
	struct Map *Map;
	struct Trigger *Triggers;
	usize TriggerCnt;
	struct Decal *Decals;
	usize DecalCnt;
};

// tables.
u16 MapList_ParTimes[MLI_END__][3] =
{
	// dummy: custom map.
	{0, 0, 0},
	
	// chapter 0.
	{0, 0, 0}, // c0e0.
	{0, 0, 0}, // c0e1.
	{0, 0, 0}, // c0e2.
	{0, 0, 0}, // c0e3.
	{0, 0, 0}, // c0e4.
	{0, 0, 0}, // c0e5.
	{0, 0, 0}, // c0e6.
	{0, 0, 0}, // c0e7.
	{0, 0, 0}, // c0e8.
	{0, 0, 0}, // c0e9.
	{0, 0, 0}, // c0e10.
	{0, 0, 0}, // c0e11.
	{0, 0, 0}, // c0e12.
	{0, 0, 0}, // c0e13.
	{0, 0, 0}, // c0e14.
	{0, 0, 0}, // c0e15.
	{0, 0, 0}, // c0e16.
	{0, 0, 0}, // c0e17.
	{0, 0, 0}, // c0e18.
	{0, 0, 0}, // c0e19.
	{0, 0, 0}, // c0e20.
	{0, 0, 0}, // c0e21.
	{0, 0, 0}, // c0e22.
	{0, 0, 0}, // c0e23.
	{0, 0, 0} // c0e24.
};

char const *MapList_Names[MLI_END__] =
{
	// dummy: custom map.
	NULL,
	
	// chapter 0.
	"The basics", // c0e0.
	"Kill tiles", // c0e1.
	"Bounce tiles", // c0e2.
	"Leap of faith", // c0e3.
	"Launch tiles", // c0e4.
	"Zoom", // c0e5.
	"Switch tiles", // c0e6.
	"Double-ended I", // c0e7.
	"Downwards dash", // c0e8.
	"Slippery tiles", // c0e9.
	"Wave in a box", // c0e10.
	"Grip tiles", // c0e11.
	"Tide in a box", // c0e12.
	"Dropper", // c0e13.
	"Rapid powerjump", // c0e14.
	"Perfect Dash", // c0e15.
	"Double-ended II", // c0e16.
	"Lopside Double", // c0e17.
	"Cube accelerator", // c0e18.
	"Switch tiles", // c0e19.
	"Casette-based", // c0e20.
	"Beam tiles", // c0e21.
	"Double-ended III", // c0e22.
	"Magnetic levitation", // c0e23.
	"The deceit" // c0e24.
};

static char const *CurCustom;
static enum MapListItem CurItem;
static struct Item ItemData[MLI_END__] =
{
	{
		// dummy: custom map.
		.Map = NULL,
	},
	
	// chapter 0.
	INCLUDE_MAP(c0e0),
	INCLUDE_MAP(c0e1),
	INCLUDE_MAP(c0e2),
	INCLUDE_MAP(c0e3),
	INCLUDE_MAP(c0e4),
	INCLUDE_MAP(c0e5),
	INCLUDE_MAP(c0e6),
	INCLUDE_MAP(c0e7),
	INCLUDE_MAP(c0e8),
	INCLUDE_MAP(c0e9),
	INCLUDE_MAP(c0e10),
	INCLUDE_MAP(c0e11),
	INCLUDE_MAP(c0e12),
	INCLUDE_MAP(c0e13),
	INCLUDE_MAP(c0e14),
	INCLUDE_MAP(c0e15),
	INCLUDE_MAP(c0e16),
	INCLUDE_MAP(c0e17),
	INCLUDE_MAP(c0e18),
	INCLUDE_MAP(c0e19),
	INCLUDE_MAP(c0e20),
	INCLUDE_MAP(c0e21),
	INCLUDE_MAP(c0e22),
	INCLUDE_MAP(c0e23),
	INCLUDE_MAP(c0e24)
};

enum MapListItem
MapList_CurrentMap(void)
{
	return CurItem;
}

void
MapList_Load(enum MapListItem Item)
{
	// init gameplay elements.
	{
		g_Map = *ItemData[Item].Map;
		
		g_TriggerCnt = 0;
		for (usize i = 0; i < ItemData[Item].TriggerCnt; ++i)
			Triggers_AddTrigger(&ItemData[Item].Triggers[i]);
		
		g_PlayerState = PS_PLAYING;
		g_Player = (struct Player)
		{
			.PosX = g_Map.PlayerSpawnX,
			.PosY = g_Map.PlayerSpawnY,
		};
		g_PlayerCapMask = (struct PlayerCapMask){0};
		
		Game_DisableSwitches();
	}
	
	// init aesthetic elements.
	{
		g_Cam = (struct Cam)
		{
			.PosX = g_Map.PlayerSpawnX,
			.PosY = g_Map.PlayerSpawnY,
			.Zoom = 1.0f,
		};
		
		g_Game.IlTimeMs = 0;
		g_Game.IlDeaths = 0;
		
		TextList_Term();
		
		g_ParticleCnt = 0;
		
		g_DecalCnt = 0;
		for (usize i = 0; i < ItemData[Item].DecalCnt; ++i)
		{
			struct Decal const *d = &ItemData[Item].Decals[i];
			Vfx_PutDecal(d->Type, d->PosX, d->PosY, d->Layer);
		}
	}
	
	// register map list item as being loaded.
	{
		CurItem = Item;
	}
}

i32
MapList_LoadCustom(char const *Path)
{
	// init gameplay elements.
	{
		if (Map_LoadFromFile(Path))
			return 1;
		
		g_PlayerState = PS_PLAYING;
		g_Player = (struct Player)
		{
			.PosX = g_Map.PlayerSpawnX,
			.PosY = g_Map.PlayerSpawnY,
		};
		g_PlayerCapMask = (struct PlayerCapMask){0};
		
		Game_DisableSwitches();
	}
	
	// init aesthetic elements.
	{
		g_Cam = (struct Cam)
		{
			.PosX = g_Map.PlayerSpawnX,
			.PosY = g_Map.PlayerSpawnY,
			.Zoom = 1.0f,
		};
		
		g_Game.IlTimeMs = 0;
		g_Game.IlDeaths = 0;
		
		TextList_Term();
		
		g_ParticleCnt = 0;
	}
	
	// register map list item as being loaded.
	{
		CurItem = MLI_CUSTOM;
		CurCustom = Path;
	}
	
	return 0;
}

void
MapList_HardReload(void)
{
	if (CurItem == MLI_CUSTOM)
	{
		free(g_Map.Data);
		MapList_LoadCustom(CurCustom);
	}
	else
		MapList_Load(CurItem);
}

void
MapList_SoftReload(void)
{
	g_PlayerState = PS_PLAYING;
	g_Player = (struct Player)
	{
		.PosX = g_Map.PlayerSpawnX,
		.PosY = g_Map.PlayerSpawnY,
	};
	
	Game_DisableSwitches();
}

void
MapList_LoadNext(void)
{
	switch (LevelEndMenuLoop())
	{
	case MR_NEXT:
		if (CurItem == MLI_CUSTOM)
			g_Game.Running = false;
		else if (CurItem == MLI_END__ - 1)
		{
			CreditsSequence();
			g_Game.Running = false;
		}
		else
			MapList_Load(CurItem + 1);
		break;
	case MR_RETRY:
		MapList_HardReload();
		break;
	default:
		break;
	}
}
