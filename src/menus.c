#include "menus.h"

#include <stdbool.h>

#include <SDL.h>

#include "conf.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "text.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

static void pause_draw_bg(void);
static void btn_quit(void);
static void btn_main_menu(void);
static void btn_exit_to_desktop(void);

static bool in_menu = false;

void
main_menu_loop(void)
{
	// TODO: implement.
}

void
level_end_menu_loop(void)
{
	ui_button_t b_next_level = ui_button_create(80, 300, "Next level", btn_quit);
	
	in_menu = true;
	while (in_menu)
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
			case SDL_MOUSEBUTTONUP:
				mouse_release_button(&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_press_button(&e);
				break;
			default:
				break;
			}
		}
		
		// update pause menu.
		{
			ui_button_update(&b_next_level);
			keybd_post_update();
			mouse_post_update();
		}
		
		// draw pause menu.
		{
			static uint8_t cbg[] = CONF_COLOR_LEVEL_END_BG;
			
			SDL_SetRenderDrawColor(g_rend, cbg[0], cbg[1], cbg[2], 255);
			SDL_RenderClear(g_rend);
			
			// draw UI elements.
			{
				uint64_t il_time_s = g_game.il_time_ms / 1000;
				uint64_t il_time_m = il_time_s / 60;
				
				static char il_buf[32];
				sprintf(il_buf,
				        "IL: %01lu:%02lu.%lu",
				        il_time_m,
				        il_time_s % 60,
				        g_game.il_time_ms % 1000);
				
				text_draw_str("Level complete", 80, 100);
				text_draw_str(g_map.name, 80, 140);
				text_draw_str(il_buf, 80, 180);
				
				ui_button_draw(&b_next_level);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

void
pause_menu_loop(void)
{
	ui_button_t b_resume = ui_button_create(80, 300, "Resume", btn_quit);
	ui_button_t b_main_menu = ui_button_create(80, 340, "Main menu", btn_main_menu);
	ui_button_t b_exit = ui_button_create(80, 380, "Exit to desktop", btn_exit_to_desktop);
	
	in_menu = true;
	while (in_menu)
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
			case SDL_MOUSEBUTTONUP:
				mouse_release_button(&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_press_button(&e);
				break;
			default:
				break;
			}
		}
		
		if (key_pressed(K_MENU))
		{
			keybd_post_update();
			mouse_post_update();
			return;
		}
		
		// update pause menu.
		{
			ui_button_update(&b_resume);
			ui_button_update(&b_main_menu);
			ui_button_update(&b_exit);
			keybd_post_update();
			mouse_post_update();
		}
		
		// draw pause menu.
		{
			pause_draw_bg();
			
			// draw UI.
			{
				text_draw_str("Paused", 80, 100);
				
				ui_button_draw(&b_resume);
				ui_button_draw(&b_main_menu);
				ui_button_draw(&b_exit);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

static void
pause_draw_bg(void)
{
	static uint8_t cbg[] = CONF_COLOR_BG, cbgs[] = CONF_COLOR_BG_SQUARE;
	static float first_square_x = 0.0f;
	static float first_square_y = 0.0f;
	
	// update square positions.
	{
		first_square_x -= CONF_BG_SQUARE_SPEED_X;
		if (first_square_x <= -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP)
			first_square_x += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
		
		first_square_y -= CONF_BG_SQUARE_SPEED_Y;
		if (first_square_y <= -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP)
			first_square_y += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	}
	
	// render background.
	SDL_SetRenderDrawColor(g_rend, cbgs[0], cbgs[1], cbgs[2], 255);
	SDL_RenderClear(g_rend);
	
	SDL_SetRenderDrawColor(g_rend, cbg[0], cbg[1], cbg[2], 255);
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
btn_quit(void)
{
	in_menu = false;
}

static void
btn_main_menu(void)
{
	// TODO: implement.
}

static void
btn_exit_to_desktop(void)
{
	exit(0);
}
