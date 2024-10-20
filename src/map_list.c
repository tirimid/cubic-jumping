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

#define INCLUDE_MAP(name) \
	{ \
		.map = &name##_map, \
		.triggers = name##_triggers, \
		.ntriggers = name##_NTRIGGERS, \
	}

struct item
{
	struct map *map;
	struct trigger *triggers;
	size_t ntriggers;
};

static char const *cur_custom;
static enum map_list_item cur_item;
static struct item item_data[MLI_END__] =
{
	{
		// dummy: custom map.
		.map = NULL,
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
map_list_load(enum map_list_item item)
{
	// init gameplay elements.
	{
		g_map = *item_data[item].map;
		
		g_ntriggers = 0;
		for (size_t i = 0; i < item_data[item].ntriggers; ++i)
			triggers_add_trigger(&item_data[item].triggers[i]);
		
		g_player_state = PS_PLAYING;
		g_player = (struct player)
		{
			.pos_x = g_map.player_spawn_x,
			.pos_y = g_map.player_spawn_y,
		};
		g_player_cap_mask = (struct player_cap_mask){0};
		
		game_disable_switches();
	}
	
	// init aesthetic elements.
	{
		g_cam = (struct cam)
		{
			.pos_x = g_map.player_spawn_x,
			.pos_y = g_map.player_spawn_y,
			.zoom = 1.0f,
		};
		
		g_game.il_time_ms = 0;
		g_game.il_deaths = 0;
		
		text_list_term();
		
		vfx_clear_particles();
	}
	
	// register map list item as being loaded.
	{
		cur_item = item;
	}
}

int
map_list_load_custom(char const *path)
{
	// init gameplay elements.
	{
		if (map_load_from_file(path))
			return 1;
		
		g_player_state = PS_PLAYING;
		g_player = (struct player)
		{
			.pos_x = g_map.player_spawn_x,
			.pos_y = g_map.player_spawn_y,
		};
		g_player_cap_mask = (struct player_cap_mask){0};
		
		game_disable_switches();
	}
	
	// init aesthetic elements.
	{
		g_cam = (struct cam)
		{
			.pos_x = g_map.player_spawn_x,
			.pos_y = g_map.player_spawn_y,
			.zoom = 1.0f,
		};
		
		g_game.il_time_ms = 0;
		g_game.il_deaths = 0;
		
		text_list_term();
		
		vfx_clear_particles();
	}
	
	// register map list item as being loaded.
	{
		cur_item = MLI_CUSTOM;
		cur_custom = path;
	}
	
	return 0;
}

void
map_list_hard_reload(void)
{
	if (cur_item == MLI_CUSTOM)
	{
		free(g_map.data);
		map_list_load_custom(cur_custom);
	}
	else
		map_list_load(cur_item);
}

void
map_list_soft_reload(void)
{
	g_player_state = PS_PLAYING;
	g_player = (struct player)
	{
		.pos_x = g_map.player_spawn_x,
		.pos_y = g_map.player_spawn_y,
	};
	
	game_disable_switches();
}

void
map_list_load_next(void)
{
	switch (level_end_menu_loop())
	{
	case MR_NEXT:
		if (cur_item == MLI_CUSTOM)
			g_game.running = false;
		else if (cur_item == MLI_END__ - 1)
		{
			// TODO: uncomment when credits sequence is done.
			//credits_sequence();
			g_game.running = false;
		}
		else
			map_list_load(cur_item + 1);
		break;
	case MR_RETRY:
		map_list_hard_reload();
		break;
	default:
		break;
	}
}
