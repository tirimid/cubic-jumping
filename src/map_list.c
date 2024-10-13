#include "map_list.h"

#include <stddef.h>

#include "cam.h"
#include "game.h"
#include "map.h"
#include "menus.h"
#include "player.h"
#include "triggers.h"

// compiled map data.
#include "cte0.hfm"
#include "cte1.hfm"
#include "cte2.hfm"
#include "c0e0.hfm"
#include "c0e1.hfm"
#include "c0e2.hfm"

typedef struct item_data
{
	map_t *map;
	trigger_t *triggers;
	size_t ntriggers;
} item_data_t;

static map_list_item_t cur_item;
static item_data_t item_data[MLI_END__] =
{
	{
		.map = &cte0_map,
		.triggers = cte0_triggers,
		.ntriggers = cte0_NTRIGGERS,
	},
	{
		.map = &cte1_map,
		.triggers = cte1_triggers,
		.ntriggers = cte1_NTRIGGERS,
	},
	{
		.map = &cte2_map,
		.triggers = cte2_triggers,
		.ntriggers = cte2_NTRIGGERS,
	},
	{
		.map = &c0e0_map,
		.triggers = c0e0_triggers,
		.ntriggers = c0e0_NTRIGGERS,
	},
	{
		.map = &c0e1_map,
		.triggers = c0e1_triggers,
		.ntriggers = c0e1_NTRIGGERS,
	},
	{
		.map = &c0e2_map,
		.triggers = c0e2_triggers,
		.ntriggers = c0e2_NTRIGGERS,
	},
};

void
map_list_load(map_list_item_t item)
{
	g_map = *item_data[item].map;
	
	g_ntriggers = 0;
	for (size_t i = 0; i < item_data[item].ntriggers; ++i)
		triggers_add_trigger(&item_data[item].triggers[i]);
	
	g_player_state = PS_PLAYING;
	g_player = (player_t)
	{
		.pos_x = g_map.player_spawn_x,
		.pos_y = g_map.player_spawn_y,
		.vel_x = 0.0f,
		.vel_y = 0.0f,
	};
	
	g_cam = (cam_t)
	{
		.pos_x = g_map.player_spawn_x,
		.pos_y = g_map.player_spawn_y,
		.zoom = 1.0f,
	};
	
	game_disable_switches();
	g_game.il_time_ms = 0;
	
	cur_item = item;
}

void
map_list_reload(void)
{
	g_player_state = PS_PLAYING;
	g_player = (player_t)
	{
		.pos_x = g_map.player_spawn_x,
		.pos_y = g_map.player_spawn_y,
		.vel_x = 0.0f,
		.vel_y = 0.0f,
	};
	
	game_disable_switches();
}

void
map_list_load_next(void)
{
	if (cur_item == MLI_END__ - 1)
		; // TODO: show game end screen upon finishing final level.
	else
	{
		level_end_menu_loop();
		map_list_load(cur_item + 1);
	}
}
