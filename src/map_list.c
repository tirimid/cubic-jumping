#include "map_list.h"

#include "cam.h"
#include "map.h"
#include "player.h"

// compiled map data.
#include "cte0.hfm"

static map_list_item_t cur_item;
static map_t const *items[MLI_END__] =
{
	&cte0,
};

map_list_item_t
map_list_cur_item(void)
{
	return cur_item;
}

void
map_list_load(map_list_item_t item)
{
	g_map = *items[item];
	
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
	
	cur_item = item;
}
