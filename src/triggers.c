#include "triggers.h"

#include "conf.h"
#include "player.h"
#include "util.h"

trigger_t g_triggers[TRIGGERS_MAX];
uint32_t g_ntriggers;

static void collide(trigger_t const *trigger);

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
triggers_draw(SDL_Renderer *rend)
{
	static uint8_t ct[] = CONF_COLOR_TRIGGER;
	
	for (uint32_t i = 0; i < g_ntriggers; ++i)
	{
		SDL_SetRenderDrawColor(rend,
		                       ct[0],
		                       ct[1],
		                       ct[2],
		                       CONF_COLOR_TRIGGER_OPACITY);
		
		relative_draw_rect(rend,
		                   g_triggers[i].pos_x,
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
