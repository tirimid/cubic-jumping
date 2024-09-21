#include "editor.h"

#include <stdlib.h>

#include <SDL.h>

#include "cam.h"
#include "conf.h"
#include "keybd.h"
#include "map.h"
#include "mouse.h"
#include "triggers.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

typedef enum edit_mode
{
	EM_TILE = 0,
	EM_TRIGGER,
	EM_PLAYER,
} edit_mode_t;

static void update_editor(void);
static void draw_bg(void);
static void draw_indicators(void);
static void btn_mode_tile(void);
static void btn_mode_trigger(void);
static void btn_mode_player(void);
static void btn_type_next(void);
static void btn_type_prev(void);
static void btn_zoom_in(void);
static void btn_zoom_out(void);

static char const *map_file;
static edit_mode_t mode = EM_TILE;
static int type = 0;

int
editor_init(char const *file)
{
	map_file = file;
	if (map_load_from_file(file))
		return 1;
	
	return 0;
}

void
editor_quit(void)
{
	free(g_map.data);
}

void
editor_main_loop(void)
{
	ui_button_t b_mode_tile = ui_button_create(10, 10, "Tile", btn_mode_tile);
	ui_button_t b_mode_trigger = ui_button_create(115, 10, "Trigger", btn_mode_trigger);
	ui_button_t b_mode_player = ui_button_create(285, 10, "Player", btn_mode_player);
	ui_button_t b_type_next = ui_button_create(435, 10, "Type>", btn_type_next);
	ui_button_t b_type_prev = ui_button_create(560, 10, "Type<", btn_type_prev);
	ui_button_t b_zoom_in = ui_button_create(10, 50, "Zoom+", btn_zoom_in);
	ui_button_t b_zoom_out = ui_button_create(135, 50, "Zoom-", btn_zoom_out);
	
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
		
		// update editor.
		do
		{
			// update UI.
			do
			{
				ui_button_update(&b_mode_tile);
				ui_button_update(&b_mode_trigger);
				ui_button_update(&b_mode_player);
				ui_button_update(&b_zoom_in);
				ui_button_update(&b_zoom_out);
				ui_button_update(&b_type_next);
				ui_button_update(&b_type_prev);
			} while (0);
			
			update_editor();
			keybd_post_update();
			mouse_post_update();
		} while (0);
		
		// draw editor.
		do
		{
			draw_bg();
			map_draw();
			map_draw_outlines();
			draw_indicators();
			
			// draw UI.
			do
			{
				ui_button_draw(&b_mode_tile);
				ui_button_draw(&b_mode_trigger);
				ui_button_draw(&b_mode_player);
				ui_button_draw(&b_zoom_in);
				ui_button_draw(&b_zoom_out);
				ui_button_draw(&b_type_next);
				ui_button_draw(&b_type_prev);
			} while (0);
			
			SDL_RenderPresent(g_rend);
		} while (0);
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

static void
update_editor(void)
{
	// move camera.
	do
	{
		float mv_horiz = key_down(K_RIGHT) - key_down(K_LEFT);
		float mv_vert = key_down(K_FALL) - key_down(K_JUMP);
		g_cam.pos_x += CONF_EDITOR_CAM_SPEED * mv_horiz;
		g_cam.pos_y += CONF_EDITOR_CAM_SPEED * mv_vert;
	} while (0);
	
	// mouse interaction based on mode.
	switch (mode)
	{
	case EM_TILE:
		// TODO: implement.
		break;
	case EM_TRIGGER:
		// TODO: implement.
		break;
	case EM_PLAYER:
	{
		int mouse_x, mouse_y;
		mouse_pos(&mouse_x, &mouse_y);
		
		if (mouse_y < CONF_EDITOR_BAR_SIZE)
			break;
		
		if (mouse_down(MB_LEFT))
		{
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = MAX(0.0f, sel_x);
			sel_y = MAX(0.0f, sel_y);
			sel_x = (int)sel_x;
			sel_y = (int)sel_y;
			
			if (sel_y < 0.0f || sel_y < 0.0f)
				break;
			
			g_map.player_spawn_x = sel_x;
			g_map.player_spawn_y = sel_y;
		}
		
		break;
	}
	}
}

static void
draw_bg(void)
{
	static uint8_t cbg[] = CONF_COLOR_BG;
	SDL_SetRenderDrawColor(g_rend, cbg[0], cbg[1], cbg[2], 255);
	SDL_RenderClear(g_rend);
}

static void
draw_indicators(void)
{
	// draw player spawn position.
	do
	{
		static uint8_t cp[] = CONF_COLOR_PLAYER;
		
		SDL_SetRenderDrawColor(g_rend, cp[0], cp[1], cp[2], 255);
		
		relative_draw_rect(g_map.player_spawn_x,
		                   g_map.player_spawn_y,
		                   CONF_PLAYER_SIZE,
		                   CONF_PLAYER_SIZE);
	} while (0);
	
	// draw hover / selection boundary.
	do
	{
		static uint8_t cb[] = CONF_COLOR_EDITOR_BOUNDARY;
		
		SDL_SetRenderDrawColor(g_rend,
		                       cb[0],
		                       cb[1],
		                       cb[2],
		                       CONF_COLOR_EDITOR_BOUNDARY_OPACITY);
		
		switch (mode)
		{
		case EM_TILE:
		case EM_PLAYER:
		{
			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = MAX(0.0f, sel_x);
			sel_y = MAX(0.0f, sel_y);
			sel_x = (int)sel_x;
			sel_y = (int)sel_y;
			
			relative_draw_rect(sel_x, sel_y, 1.0f, 1.0f);
			
			break;
		}
		case EM_TRIGGER:
			// TODO: implement.
			break;
		}
	} while (0);
	
	// draw editor bar.
	do
	{
		static uint8_t cbgs[] = CONF_COLOR_BG_SQUARE;
		
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = CONF_WND_WIDTH,
			.h = CONF_EDITOR_BAR_SIZE,
		};
		
		SDL_SetRenderDrawColor(g_rend, cbgs[0], cbgs[1], cbgs[2], 255);
		SDL_RenderFillRect(g_rend, &r);
	} while (0);
	
	// draw current type indicator.
	do
	{
		static uint8_t co[] = CONF_COLOR_OUTLINE;
		
		SDL_Rect r =
		{
			.x = CONF_WND_WIDTH - CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.y = 0,
			.w = CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.h = CONF_EDITOR_TYPE_INDICATOR_SIZE,
		};
		
		switch (mode)
		{
		case EM_TILE:
		{
			uint8_t const *col = map_tile_color(type);
			SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
			SDL_RenderFillRect(g_rend, &r);
			SDL_SetRenderDrawColor(g_rend, co[0], co[1], co[2], 255);
			SDL_RenderDrawRect(g_rend, &r);
			break;
		}
		case EM_TRIGGER:
		{
			uint8_t const *col = trigger_color(type);
			SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
			SDL_RenderFillRect(g_rend, &r);
			SDL_SetRenderDrawColor(g_rend, co[0], co[1], co[2], 255);
			SDL_RenderDrawRect(g_rend, &r);
			break;
		}
		default:
			break;
		}
	} while (0);
}

static void
btn_mode_tile(void)
{
	mode = EM_TILE;
	type = 0;
}

static void
btn_mode_trigger(void)
{
	mode = EM_TRIGGER;
	type = 0;
}

static void
btn_mode_player(void)
{
	mode = EM_PLAYER;
}

static void
btn_type_next(void)
{
	switch (mode)
	{
	case EM_TILE:
		type = type == MTT_END__ - 1 ? 0 : type + 1;
		break;
	case EM_TRIGGER:
		type = type == TT_END__ - 1 ? 0 : type + 1;
		break;
	default:
		break;
	}
}

static void
btn_type_prev(void)
{
	switch (mode)
	{
	case EM_TILE:
		type = type == 0 ? MTT_END__ - 1 : type - 1;
		break;
	case EM_TRIGGER:
		type = type == 0 ? TT_END__ - 1 : type - 1;
		break;
	default:
		break;
	}
}

static void
btn_zoom_in(void)
{
	g_cam.zoom += CONF_EDITOR_CAM_ZOOM;
	g_cam.zoom = MIN(g_cam.zoom, CONF_CAM_MAX_ZOOM);
}

static void
btn_zoom_out(void)
{
	g_cam.zoom -= CONF_EDITOR_CAM_ZOOM;
	g_cam.zoom = MAX(g_cam.zoom, CONF_CAM_MIN_ZOOM);
}
