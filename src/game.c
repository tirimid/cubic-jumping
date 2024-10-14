#include "game.h"

#include <stdlib.h>

#include <SDL.h>

#include "cam.h"
#include "conf.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "menus.h"
#include "player.h"
#include "text.h"
#include "text_list.h"
#include "triggers.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

game g_game;

static void draw_bg(void);
static void fill_out_of_bounds(void);
static void draw_indicators(void);

void
game_loop(void)
{
	for (;;)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				exit(0);
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
		
		if (key_pressed(K_MENU))
		{
			keybd_post_update();
			if (pause_menu_loop() == MR_EXIT)
				return;
			continue;
		}
		
		// update game.
		{
			player_update();
			triggers_update();
			vfx_update();
			cam_update();
			text_list_update();
			keybd_post_update();
		}
		
		// draw game.
		{
			draw_bg();
			fill_out_of_bounds();
			map_draw();
			vfx_draw();
			player_draw();
#if CONF_SHOW_TRIGGERS
			triggers_draw();
#endif
			text_list_draw();
			draw_indicators();
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
		
		g_game.il_time_ms += CONF_TICK_MS;
	}
}

void
game_disable_switches(void)
{
	g_game.off_switches = 0;
	for (size_t i = 0, size = g_map.size_x * g_map.size_y; i < size; ++i)
	{
		if (g_map.data[i].type == MTT_SWITCH_ON)
		{
			g_map.data[i].type = MTT_SWITCH_OFF;
			++g_game.off_switches;
		}
		else if (g_map.data[i].type == MTT_SWITCH_OFF)
			++g_game.off_switches;
	}
}

static void
draw_bg(void)
{
	static uint8_t cbg[] = CONF_COLOR_BG, cbgs[] = CONF_COLOR_BG_SQUARE;
	static float first_square_x = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	static float first_square_y = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	
	// update square positions.
	{
		first_square_x += CONF_BG_SQUARE_SPEED_X;
		if (first_square_x >= 0.0f)
			first_square_x -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
		
		first_square_y += CONF_BG_SQUARE_SPEED_Y;
		if (first_square_y >= 0.0f)
			first_square_y -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	}
	
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

static void
fill_out_of_bounds(void)
{
	int scr_lbx, scr_lby;
	game_to_screen_coord(&scr_lbx, &scr_lby, 0.0f, 0.0f);
	
	int scr_ubx, scr_uby;
	game_to_screen_coord(&scr_ubx, &scr_uby, g_map.size_x, g_map.size_y);
	
	// set OOB cover draw color.
	{
		static uint8_t cg[] = CONF_COLOR_GROUND;
		SDL_SetRenderDrawColor(g_rend, cg[0], cg[1], cg[2], 255);
	}
	
	// draw left OOB cover.
	if (scr_lbx > 0)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = scr_lbx,
			.h = CONF_WND_HEIGHT,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw right OOB cover.
	if (scr_ubx < CONF_WND_WIDTH)
	{
		SDL_Rect r =
		{
			.x = scr_ubx,
			.y = 0,
			.w = CONF_WND_WIDTH - scr_ubx,
			.h = CONF_WND_HEIGHT,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw top OOB cover.
	if (scr_lby > 0)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = CONF_WND_WIDTH,
			.h = scr_lby,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw bottom OOB cover.
	if (scr_uby < CONF_WND_HEIGHT)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = scr_uby,
			.w = CONF_WND_WIDTH,
			.h = CONF_WND_HEIGHT - scr_uby,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
}

static void
draw_indicators(void)
{
	// draw IL timer.
	{
		uint64_t il_time_s = g_game.il_time_ms / 1000;
		uint64_t il_time_m = il_time_s / 60;
		
		static char buf[32];
		sprintf(buf,
		        "%01lu:%02lu.%lu",
		        il_time_m,
		        il_time_s % 60,
		        g_game.il_time_ms % 1000);
		
		text_draw_str(buf, CONF_TIMER_OFF_X, CONF_TIMER_OFF_Y);
	}
}
