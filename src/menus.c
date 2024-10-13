#include "menus.h"

#include <stdbool.h>

#include <SDL.h>

#include "conf.h"
#include "input.h"
#include "text.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

static void pause_draw_bg(void);
static void pause_btn_resume(void);
static void pause_btn_main_menu(void);
static void pause_btn_exit(void);

static bool paused = false;

void
main_menu_loop(void)
{
	// TODO: implement.
}

void
pause_menu_loop(void)
{
	ui_button_t b_resume = ui_button_create(30, 300, "Resume", pause_btn_resume);
	ui_button_t b_main_menu = ui_button_create(30, 340, "Main menu", pause_btn_main_menu);
	ui_button_t b_exit = ui_button_create(30, 380, "Exit", pause_btn_exit);
	
	paused = true;
	while (paused)
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
				text_draw_str("Paused", 30, 100);
				
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
pause_btn_resume(void)
{
	paused = false;
}

static void
pause_btn_main_menu(void)
{
	// TODO: implement.
}

static void
pause_btn_exit(void)
{
	exit(0);
}
