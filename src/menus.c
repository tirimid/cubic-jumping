#include "menus.h"

#include <setjmp.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "text.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

static void main_draw_bg(void);
static void pause_draw_bg(void);
static void btn_exit_menu(void);
static void btn_exit_to_desktop(void);
static void btn_play_from_beginning(void);
static void btn_req_exit(void);
static void btn_req_next(void);
static void btn_req_retry(void);

static bool in_menu = false;
static menu_request req = MR_NONE;

void
main_menu_loop(void)
{
	ui_button b_continue = ui_button_create(80, 380, "Continue", NULL);
	ui_button b_play = ui_button_create(80, 420, "Play from beginning", btn_play_from_beginning);
	ui_button b_play_custom = ui_button_create(80, 460, "Play custom level", NULL);
	ui_button b_editor = ui_button_create(80, 500, "Level editor", NULL);
	ui_button b_exit = ui_button_create(80, 540, "Exit to desktop", btn_exit_to_desktop);
	
	// `in_menu` is irrelevant for the main menu since it is the main launch
	// screen for game functionality, and it doesn't really make sense to
	// "quit" the main menu.
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
		
		// update main menu.
		{
			ui_button_update(&b_continue);
			ui_button_update(&b_play);
			ui_button_update(&b_play_custom);
			ui_button_update(&b_editor);
			ui_button_update(&b_exit);
			keybd_post_update();
			mouse_post_update();
		}
		
		// draw pause menu.
		{
			main_draw_bg();
			
			// draw UI.
			{
				text_draw_str("CUBIC JUMPING", 80, 60);
				
				ui_button_draw(&b_continue);
				ui_button_draw(&b_play);
				ui_button_draw(&b_play_custom);
				ui_button_draw(&b_editor);
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

menu_request
level_end_menu_loop(void)
{
	ui_button b_next = ui_button_create(80, 380, "Next level", btn_req_next);
	ui_button b_retry = ui_button_create(80, 420, "Retry level", btn_req_retry);
	
	in_menu = true;
	req = MR_NONE;
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
		
		// update level end menu.
		{
			ui_button_update(&b_next);
			ui_button_update(&b_retry);
			keybd_post_update();
			mouse_post_update();
		}
		
		// draw level end menu.
		{
			static uint8_t cbg[] = CONF_COLOR_LEVEL_END_BG;
			
			SDL_SetRenderDrawColor(g_rend, cbg[0], cbg[1], cbg[2], 255);
			SDL_RenderClear(g_rend);
			
			// draw UI elements.
			{
				uint64_t il_time_s = g_game.il_time_ms / 1000;
				uint64_t il_time_m = il_time_s / 60;
				uint64_t total_time_s = g_game.total_time_ms / 1000;
				uint64_t total_time_m = total_time_s / 60;
				
				static char time_buf[64];
				sprintf(time_buf,
				        "Time: %01lu:%02lu.%lu (%01lu:%02lu.%lu)",
				        il_time_m,
				        il_time_s % 60,
				        g_game.il_time_ms % 1000 / 10,
				        total_time_m,
				        total_time_s % 60,
				        g_game.total_time_ms % 1000 / 10);
				
				static char deaths_buf[64];
				sprintf(deaths_buf,
				        "Deaths: %u (%u)",
				        g_game.il_deaths,
				        g_game.total_deaths);
				
				text_draw_str("Level complete", 80, 60);
				
				text_draw_str(g_map.name, 80, 140);
				text_draw_str(time_buf, 80, 180);
				text_draw_str(deaths_buf, 80, 220);
				
				ui_button_draw(&b_next);
				ui_button_draw(&b_retry);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
	
	return req;
}

menu_request
pause_menu_loop(void)
{
	ui_button b_resume = ui_button_create(80, 380, "Resume", btn_exit_menu);
	ui_button b_main_menu = ui_button_create(80, 420, "Main menu", btn_req_exit);
	ui_button b_exit = ui_button_create(80, 460, "Exit to desktop", btn_exit_to_desktop);
	
	in_menu = true;
	req = MR_NONE;
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
		
		if (key_pressed(CONF_KEY_MENU))
		{
			keybd_post_update();
			mouse_post_update();
			return req;
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
				text_draw_str("Paused", 80, 60);
				
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
	
	return req;
}

static void
main_draw_bg(void)
{
	// draw basic background squares effect.
	{
		pause_draw_bg();
	}
	
	// draw DVD screensaver.
	{
		static uint8_t cbgda[] = CONF_COLOR_BG_DVD_A;
		static uint8_t cbgdb[] = CONF_COLOR_BG_DVD_B;
		
		static int pos_x = 0, pos_y = 0;
		static int speed_x = CONF_BG_DVD_SPEED, speed_y = CONF_BG_DVD_SPEED;
		static float col_lerp = 0.0f, col_speed = CONF_BG_DVD_COL_SPEED;
		
		pos_x += speed_x;
		pos_y += speed_y;
		
		if (pos_x < 0 || pos_x + CONF_BG_DVD_SIZE >= CONF_WND_WIDTH)
			speed_x *= -1;
		if (pos_y < 0 || pos_y + CONF_BG_DVD_SIZE >= CONF_WND_HEIGHT)
			speed_y *= -1;
		
		col_lerp += col_speed;
		if (col_lerp < 0.0f || col_lerp > 1.0f)
			col_speed *= -1.0f;
		col_lerp = CLAMP(0.0f, col_lerp, 1.0f);
		
		SDL_Rect r =
		{
			.x = pos_x,
			.y = pos_y,
			.w = CONF_BG_DVD_SIZE,
			.h = CONF_BG_DVD_SIZE,
		};
		
		uint8_t col[3] =
		{
			lerp(cbgda[0], cbgdb[0], col_lerp),
			lerp(cbgda[1], cbgdb[1], col_lerp),
			lerp(cbgda[2], cbgdb[2], col_lerp),
		};
		
		SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
		SDL_RenderFillRect(g_rend, &r);
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
btn_exit_menu(void)
{
	in_menu = false;
}

static void
btn_exit_to_desktop(void)
{
	exit(0);
}

static void
btn_play_from_beginning(void)
{
	map_list_load(MLI_C0E0);
	g_game.total_time_ms = 0;
	game_loop();
}

static void
btn_req_exit(void)
{
	req = MR_EXIT;
	in_menu = false;
}

static void
btn_req_next(void)
{
	req = MR_NEXT;
	in_menu = false;
}

static void
btn_req_retry(void)
{
	req = MR_RETRY;
	in_menu = false;
}
