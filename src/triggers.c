#include "triggers.h"

#include <SDL.h>

#include "conf.h"
#include "player.h"
#include "util.h"
#include "wnd.h"

trigger_t g_triggers[TRIGGERS_MAX];
uint32_t g_ntriggers;

static void collide(trigger_t const *trigger);

uint8_t const *
trigger_color(trigger_type_t type)
{
	static uint8_t colors[TT_END__][3] =
	{
		CONF_COLOR_BG,
		CONF_COLOR_TRIGGER_MSG,
		CONF_COLOR_TRIGGER_KILL,
	};
	
	return colors[type];
}

void
triggers_add_trigger(trigger_type_t type,
                     float px,
                     float py,
                     float sx,
                     float sy)
{
	// TODO: implement.
}

void
triggers_update(void)
{
	for (uint32_t i = 0; i < g_ntriggers; ++i)
	{
		if (g_player.pos_x + CONF_PLAYER_SIZE >= g_triggers[i].pos_x
		    && g_player.pos_x < g_triggers[i].pos_x + g_triggers[i].size_x
		    && g_player.pos_y + CONF_PLAYER_SIZE >= g_triggers[i].pos_y
		    && g_player.pos_y < g_triggers[i].pos_y + g_triggers[i].size_y)
		{
			collide(&g_triggers[i]);
		}
	}
}

void
triggers_draw(void)
{
	for (uint32_t i = 0; i < g_ntriggers; ++i)
	{
		uint8_t const *col = trigger_color(g_triggers[i].type);
		SDL_SetRenderDrawColor(g_rend,
		                       col[0],
		                       col[1],
		                       col[2],
		                       CONF_COLOR_TRIGGER_OPACITY);
		
		relative_draw_rect(g_triggers[i].pos_x,
		                   g_triggers[i].pos_y,
		                   g_triggers[i].size_x,
		                   g_triggers[i].size_y);
	}
}

static void
collide(trigger_t const *trigger)
{
	// TODO: implement.
}
