#include "menus.h"

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "conf.h"
#include "editor.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "options.h"
#include "sound.h"
#include "text.h"
#include "triggers.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

#define MAX_LEVEL_SEL_PATH_SIZE 512

static void main_draw_bg(void);
static void pause_draw_bg(void);
static void btn_exit_menu(void);
static void btn_exit_to_desktop(void);
static void btn_play_from_beginning(void);
static void btn_play_edit_custom_level(void);
static void btn_play_custom_level(void);
static void btn_edit_custom_level(void);
static void btn_force_retry(void);
static void btn_main_menu(void);
static void btn_options(void);
static void btn_detect_key_left(void);
static void btn_detect_key_right(void);
static void btn_detect_key_jump(void);
static void btn_detect_key_dash_down(void);
static void btn_detect_key_powerjump(void);
static void btn_detect_key_menu(void);
static void sldr_sfx_volume(float vol);
static void btn_exit_options_menu(void);
static void btn_req_next(void);
static void btn_req_retry(void);

static bool in_menu = false;
static menu_request req = MR_NONE;
static char custom_level_path[MAX_LEVEL_SEL_PATH_SIZE];

void
main_menu_loop(void)
{
	ui_button b_continue = ui_button_create(80, 380, "Continue", NULL);
	ui_button b_play = ui_button_create(80, 420, "Play from beginning", btn_play_from_beginning);
	ui_button b_play_custom = ui_button_create(80, 460, "Play or edit custom level", btn_play_edit_custom_level);
	ui_button b_editor = ui_button_create(80, 500, "Options", btn_options);
	ui_button b_exit = ui_button_create(80, 540, "Exit to desktop", btn_exit_to_desktop);
	
	// `in_menu` is irrelevant for the main menu since it is the main launch
	// screen for game functionality, and it doesn't really make sense to
	// "quit" the main menu.
	for (;;)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
 		input_handle_events();
		
		// update main menu.
		{
			ui_button_update(&b_continue);
			ui_button_update(&b_play);
			ui_button_update(&b_play_custom);
			ui_button_update(&b_editor);
			ui_button_update(&b_exit);
			input_post_update();
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
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
}

void
custom_level_select_menu_loop(void)
{
	ui_text_field tf_path = ui_text_field_create(80, 380, 20, custom_level_path, MAX_LEVEL_SEL_PATH_SIZE - 1);
	ui_button b_play_level = ui_button_create(80, 420, "Play custom level", btn_play_custom_level);
	ui_button b_edit_level = ui_button_create(80, 460, "Edit custom level", btn_edit_custom_level);
	ui_button b_back = ui_button_create(80, 500, "Back", btn_exit_menu);
	
	in_menu = true;
	while (in_menu)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		input_handle_events();
		
		// update level select menu.
		{
			if (key_pressed(g_options.k_menu))
				in_menu = false;
			
			ui_text_field_update(&tf_path);
			ui_button_update(&b_play_level);
			ui_button_update(&b_edit_level);
			ui_button_update(&b_back);
			input_post_update();
		}
		
		// draw level select menu.
		{
			main_draw_bg();
			
			// draw UI elements.
			{
				text_draw_str("Select level", 80, 60);
				
				ui_text_field_draw(&tf_path);
				ui_button_draw(&b_play_level);
				ui_button_draw(&b_edit_level);
				ui_button_draw(&b_back);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
}

menu_request
level_end_menu_loop(void)
{
	ui_button b_next = ui_button_create(80, 380, "Continue", btn_req_next);
	ui_button b_retry = ui_button_create(80, 420, "Retry level", btn_req_retry);
	ui_button b_main_menu = ui_button_create(80, 460, "Main menu", btn_main_menu);
	
	in_menu = true;
	req = MR_NONE;
	while (in_menu)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
 		input_handle_events();
		
		// update level end menu.
		{
			ui_button_update(&b_next);
			ui_button_update(&b_retry);
			ui_button_update(&b_main_menu);
			input_post_update();
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
				        "Time: %lu:%02lu.%02lu (%lu:%02lu.%02lu)",
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
				ui_button_draw(&b_main_menu);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
	
	return req;
}

void
pause_menu_loop(void)
{
	ui_button b_resume = ui_button_create(80, 380, "Resume", btn_exit_menu);
	ui_button b_retry = ui_button_create(80, 420, "Retry level", btn_force_retry);
	ui_button b_main_menu = ui_button_create(80, 460, "Main menu", btn_main_menu);
	
	in_menu = true;
	while (in_menu)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		input_handle_events();
		
		// update pause menu.
		{
			if (key_pressed(g_options.k_menu))
				in_menu = false;
			
			ui_button_update(&b_resume);
			ui_button_update(&b_retry);
			ui_button_update(&b_main_menu);
			input_post_update();
		}
		
		// draw pause menu.
		{
			pause_draw_bg();
			
			// draw UI.
			{
				text_draw_str("Paused", 80, 60);
				
				ui_button_draw(&b_resume);
				ui_button_draw(&b_retry);
				ui_button_draw(&b_main_menu);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
}

void
options_menu_loop(void)
{
	ui_button b_k_left = ui_button_create(80, 140, "[Left]", btn_detect_key_left);
	ui_button b_k_right = ui_button_create(80, 180, "[Right]", btn_detect_key_right);
	ui_button b_k_jump = ui_button_create(80, 220, "[Jump]", btn_detect_key_jump);
	ui_button b_k_dash_down = ui_button_create(80, 260, "[Dash down]", btn_detect_key_dash_down);
	ui_button b_k_powerjump = ui_button_create(80, 300, "[Powerjump]", btn_detect_key_powerjump);
	ui_button b_k_menu = ui_button_create(80, 340, "[Menu]", btn_detect_key_menu);
	ui_slider s_sfx_volume = ui_slider_create(400, 390, 200, 20, g_options.sfx_volume, sldr_sfx_volume);
	ui_button b_back = ui_button_create(80, 420, "Back", btn_exit_options_menu);
	
	in_menu = true;
	while (in_menu)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		input_handle_events();
		
		// update options menu.
		{
			ui_button_update(&b_k_left);
			ui_button_update(&b_k_right);
			ui_button_update(&b_k_jump);
			ui_button_update(&b_k_dash_down);
			ui_button_update(&b_k_powerjump);
			ui_button_update(&b_k_menu);
			ui_slider_update(&s_sfx_volume);
			ui_button_update(&b_back);
			
			input_post_update();
		}
		
		// draw options menu.
		{
			main_draw_bg();
			
			// draw UI.
			{
				text_draw_str("Options", 80, 60);
				
				ui_button_draw(&b_k_left);
				ui_button_draw(&b_k_right);
				ui_button_draw(&b_k_jump);
				ui_button_draw(&b_k_dash_down);
				ui_button_draw(&b_k_powerjump);
				ui_button_draw(&b_k_menu);
				ui_slider_draw(&s_sfx_volume);
				ui_button_draw(&b_back);
				
				text_draw_str(SDL_GetKeyName(g_options.k_left), 450, 140);
				text_draw_str(SDL_GetKeyName(g_options.k_right), 450, 180);
				text_draw_str(SDL_GetKeyName(g_options.k_jump), 450, 220);
				text_draw_str(SDL_GetKeyName(g_options.k_dash_down), 450, 260);
				text_draw_str(SDL_GetKeyName(g_options.k_powerjump), 450, 300);
				text_draw_str(SDL_GetKeyName(g_options.k_menu), 450, 340);
				text_draw_str("SFX volume", 80, 385);
			}
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
}

SDL_Keycode
key_detect_menu_loop(void)
{
	for (;;)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		input_handle_events();
		
		// update key detection menu.
		{
			for (SDL_Keycode i = 0; i < 128; ++i)
			{
				if (key_pressed(i))
					return i;
			}
			
			for (SDL_Keycode i = 128; i < 1024; ++i)
			{
				if (key_pressed(i))
					return i - 128 | 1 << 30;
			}
			
			input_post_update();
		}
		
		// draw key detection menu.
		{
			static uint8_t cawbg[] = CONF_COLOR_AWAITING_INPUT_BG;
			SDL_SetRenderDrawColor(g_rend, cawbg[0], cawbg[1], cawbg[2], 255);
			SDL_RenderClear(g_rend);
			
			text_draw_str("Awaiting input...", 200, 280);
			
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		SDL_Delay(tick_time_left * (tick_time_left > 0));
	}
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
	g_game.total_deaths = 0;
	g_game.running = true;
	game_loop();
}

static void
btn_play_edit_custom_level(void)
{
	custom_level_select_menu_loop();
}

static void
btn_play_custom_level(void)
{
	if (map_list_load_custom(custom_level_path))
		return;
	
	g_game.total_time_ms = 0;
	g_game.total_deaths = 0;
	g_game.running = true;
	game_loop();
	
	free(g_map.data);
	g_ntriggers = 0;
}

static void
btn_edit_custom_level(void)
{
	struct stat stat_buf;
	if (stat(custom_level_path, &stat_buf))
	{
		// determine map name based on file path.
		char name[MAX_LEVEL_SEL_PATH_SIZE + 1] = {0};
		{
			size_t len = strlen(custom_level_path);
			
			size_t first = len;
			while (first > 0 && custom_level_path[first - 1] != '/')
				--first;
			
			size_t last = first;
			while (last < len
			       && strncmp(&custom_level_path[last], ".hfm", 4))
			{
				++last;
			}
			
			strncpy(name, &custom_level_path[first], last - first);
		}
		
		if (!name[0])
		{
			log_err("menus: could not determine name for new map!");
			return;
		}
		
		// try to create map file if doesn't exist.
		if (map_create_file(custom_level_path, name))
			return;
	}
	
	if (editor_init(custom_level_path))
		return;
	
	editor_loop();
	
	free(g_map.data);
	g_ntriggers = 0;
}

static void
btn_force_retry(void)
{
	in_menu = false;
	map_list_hard_reload();
}

static void
btn_main_menu(void)
{
	in_menu = false;
	g_game.running = false;
}

static void
btn_options(void)
{
	options_menu_loop();
}

static void
btn_detect_key_left(void)
{
	g_options.k_left = key_detect_menu_loop();
}

static void
btn_detect_key_right(void)
{
	g_options.k_right = key_detect_menu_loop();
}

static void
btn_detect_key_jump(void)
{
	g_options.k_jump = key_detect_menu_loop();
}

static void
btn_detect_key_dash_down(void)
{
	g_options.k_dash_down = key_detect_menu_loop();
}

static void
btn_detect_key_powerjump(void)
{
	g_options.k_powerjump = key_detect_menu_loop();
}

static void
btn_detect_key_menu(void)
{
	g_options.k_menu = key_detect_menu_loop();
}

static void
sldr_sfx_volume(float vol)
{
	g_options.sfx_volume = vol;
	sound_set_sfx_volume(vol);
}

static void
btn_exit_options_menu(void)
{
	options_write_to_file(CONF_OPTIONS_FILE);
	in_menu = false;
}

static void
btn_req_next(void)
{
	in_menu = false;
	req = MR_NEXT;
}

static void
btn_req_retry(void)
{
	in_menu = false;
	req = MR_RETRY;
}
