#include "triggers.h"

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "conf.h"
#include "player.h"
#include "text.h"
#include "text_list.h"
#include "util.h"
#include "wnd.h"

trigger_t g_triggers[TRIGGERS_MAX];
size_t g_ntriggers = 0;

static void collide(trigger_t const *trigger, size_t ind);

uint8_t const *
trigger_color(trigger_type_t type)
{
	static uint8_t colors[TT_END__][3] =
	{
		CONF_COLOR_TRIGGER_MSG,
		CONF_COLOR_TRIGGER_KILL,
		CONF_COLOR_TRIGGER_MSG_TERM,
	};
	
	return colors[type];
}

void
triggers_add_trigger(trigger_t const *trigger)
{
	if (g_ntriggers < TRIGGERS_MAX)
		g_triggers[g_ntriggers++] = *trigger;
}

void
triggers_rm_trigger(size_t ind)
{
	if (g_ntriggers == 0)
		return;
	
	memmove(&g_triggers[ind],
	        &g_triggers[ind + 1],
	        sizeof(trigger_t) * (g_ntriggers - ind - 1));
	--g_ntriggers;
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
			collide(&g_triggers[i], i);
		}
	}
}

void
triggers_draw(void)
{
	for (uint32_t i = 0; i < g_ntriggers; ++i)
	{
		trigger_t const *t = &g_triggers[i];
		
		// draw bounding box.
		{
			uint8_t const *col = trigger_color(t->type);
			SDL_SetRenderDrawColor(g_rend,
			                       col[0],
			                       col[1],
			                       col[2],
			                       CONF_COLOR_TRIGGER_OPACITY);
		
			relative_draw_rect(t->pos_x, t->pos_y, t->size_x, t->size_y);
		}
		
		// draw argument text.
		{
			int scr_x, scr_y;
			game_to_screen_coord(&scr_x, &scr_y, t->pos_x, t->pos_y);
			
			static char buf[32];
			sprintf(buf, "%x", t->arg);
			
			text_draw_str(buf, scr_x, scr_y);
		}
	}
}

static void
collide(trigger_t const *trigger, size_t ind)
{
	// perform trigger functionality.
	switch (trigger->type)
	{
	case TT_MSG:
		if (trigger->arg < TLI_END__)
			text_list_enqueue(trigger->arg);
		break;
	case TT_KILL:
		player_die();
		break;
	case TT_MSG_TERM:
		text_list_term();
		break;
	default:
		break;
	}
	
	if (trigger->single_use)
		triggers_rm_trigger(ind);
}
