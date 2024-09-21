#include "game.h"

#include <stdlib.h>

#include <SDL.h>

#include "cam.h"
#include "conf.h"
#include "keybd.h"
#include "map.h"
#include "map_list.h"
#include "player.h"
#include "text.h"
#include "triggers.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

static void draw_bg(void);

void
game_init(void)
{
	map_list_load(MLI_CTE0);
}

void
game_main_loop(void)
{
	text_box_show("hello wond", 500);
	
	for (;;)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				return;
			case SDL_KEYDOWN:
				if (!e.key.repeat)
					keybd_set_key_state(&e, true);
				break;
			case SDL_KEYUP:
				if (!e.key.repeat)
					keybd_set_key_state(&e, false);
				break;
			default:
				break;
			}
		}
		
		// update game.
		do
		{
			player_update();
			triggers_update();
			vfx_update();
			cam_update();
			text_box_update();
			keybd_post_update();
		} while (0);
		
		// draw game.
		do
		{
			draw_bg();
			map_draw();
			vfx_draw();
			player_draw();
#if CONF_SHOW_TRIGGERS
			triggers_draw();
#endif
			text_box_draw();
			SDL_RenderPresent(g_rend);
		} while (0);
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

static void
draw_bg(void)
{
	static uint8_t cbg[] = CONF_COLOR_BG, cbgs[] = CONF_COLOR_BG_SQUARE;
	static float first_square_x = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	static float first_square_y = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	
	// update square positions.
	first_square_x += CONF_BG_SQUARE_SPEED_X;
	if (first_square_x >= 0.0f)
		first_square_x -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	
	first_square_y += CONF_BG_SQUARE_SPEED_Y;
	if (first_square_y >= 0.0f)
		first_square_y -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	
	// render background.
	SDL_SetRenderDrawColor(g_rend, cbg[0], cbg[1], cbg[2], 255);
	SDL_RenderClear(g_rend);
	
	SDL_SetRenderDrawColor(g_rend, cbgs[0], cbgs[1], cbgs[2], 255);
	for (float x = first_square_x; x < CONF_WND_WIDTH; x += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP)
	{
		for (float y = first_square_y; y < CONF_WND_HEIGHT; y += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP)
		{
			SDL_Rect square =
			{
				.x = x,
				.y = y,
				.w = CONF_BG_SQUARE_SIZE,
				.h = CONF_BG_SQUARE_SIZE,
			};
			SDL_RenderFillRect(g_rend, &square);
		}
	}
}
