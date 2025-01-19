#include "map_list.h"

#include <stddef.h>
#include <stdlib.h>

#include "cam.h"
#include "game.h"
#include "map.h"
#include "menus.h"
#include "player.h"
#include "sequences.h"
#include "text_list.h"
#include "triggers.h"
#include "vfx.h"

// compiled map data.
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

#define INCLUDE_MAP(Name) \
	{ \
		.Map = &Name##_map, \
		.Triggers = Name##_triggers, \
		.TriggerCnt = Name##_NTRIGGERS, \
	}

struct Item
{
	struct Map *Map;
	struct Trigger *Triggers;
	size_t TriggerCnt;
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
};

void
MapList_Load(enum MapListItem Item)
{
	// init gameplay elements.
	{
		g_Map = *ItemData[Item].Map;
		
		g_TriggerCnt = 0;
		for (size_t i = 0; i < ItemData[Item].TriggerCnt; ++i)
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
		
		Vfx_ClearParticles();
	}
	
	// register map list item as being loaded.
	{
		CurItem = Item;
	}
}

int
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
		
		Vfx_ClearParticles();
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
			// TODO: uncomment when credits sequence is done.
			//credits_sequence();
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
