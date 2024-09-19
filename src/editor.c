#include "editor.h"

#include <stdlib.h>

#include <SDL.h>

#include "cam.h"
#include "conf.h"
#include "keybd.h"
#include "map.h"
#include "triggers.h"
#include "ui.h"
#include "util.h"

typedef enum edit_mode
{
	EM_TILE = 0,
	EM_TRIGGER,
} edit_mode_t;

static void update_editor(void);
static void draw_bg(void);
static void draw_indicators(void);
static void btn_mode_tile(void);
static void btn_mode_trigger(void);
static void btn_type_next(void);
static void btn_type_prev(void);
static void btn_zoom_in(void);
static void btn_zoom_out(void);

static SDL_Window *wnd;
static SDL_Renderer *rend;
static char const *map_file;
static edit_mode_t mode = EM_TILE;
static int type = 0;

int
editor_init(char const *file)
{
	atexit(editor_quit);
	
	wnd = SDL_CreateWindow(CONF_WND_TITLE,
	                       SDL_WINDOWPOS_UNDEFINED,
	                       SDL_WINDOWPOS_UNDEFINED,
	                       CONF_WND_WIDTH,
	                       CONF_WND_HEIGHT,
	                       CONF_WND_FLAGS);
	if (!wnd)
	{
		log_err("editor: failed to create window: %s\n", SDL_GetError());
		return 1;
	}
	
	rend = SDL_CreateRenderer(wnd, -1, CONF_REND_FLAGS);
	if (!rend)
	{
		log_err("editor: failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	
	if (map_load_from_file(file))
		return 1;
	
	map_file = file;
	
	return 0;
}

void
editor_quit(void)
{
	if (rend)
		SDL_DestroyRenderer(rend);
	if (wnd)
		SDL_DestroyWindow(wnd);
}

void
editor_main_loop(void)
{
	ui_button_t b_mode_tile = ui_button_create(10, 10, "Tile", btn_mode_tile);
	ui_button_t b_mode_trigger = ui_button_create(115, 10, "Trigger", btn_mode_trigger);
	ui_button_t b_type_next = ui_button_create(285, 10, "Type>", btn_type_next);
	ui_button_t b_type_prev = ui_button_create(410, 10, "Type<", btn_type_prev);
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
			case SDL_MOUSEBUTTONDOWN:
				ui_button_proc_event(&b_mode_tile, &e);
				ui_button_proc_event(&b_mode_trigger, &e);
				ui_button_proc_event(&b_zoom_in, &e);
				ui_button_proc_event(&b_zoom_out, &e);
				ui_button_proc_event(&b_type_next, &e);
				ui_button_proc_event(&b_type_prev, &e);
				break;
			default:
				break;
			}
		}
		
		// update editor.
		{
			// update UI.
			{
				ui_button_update(&b_mode_tile);
				ui_button_update(&b_mode_trigger);
				ui_button_update(&b_zoom_in);
				ui_button_update(&b_zoom_out);
				ui_button_update(&b_type_next);
				ui_button_update(&b_type_prev);
			}
			
			update_editor();
			keybd_post_update();
		}
		
		// draw editor.
		{
			draw_bg();
			map_draw(rend);
			map_draw_outlines(rend);
			draw_indicators();
			
			// draw UI.
			{
				ui_button_draw(rend, &b_mode_tile);
				ui_button_draw(rend, &b_mode_trigger);
				ui_button_draw(rend, &b_zoom_in);
				ui_button_draw(rend, &b_zoom_out);
				ui_button_draw(rend, &b_type_next);
				ui_button_draw(rend, &b_type_prev);
			}
			
			SDL_RenderPresent(rend);
		}
		
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
	{
		float mv_horiz = key_down(K_RIGHT) - key_down(K_LEFT);
		float mv_vert = key_down(K_FALL) - key_down(K_JUMP);
		g_cam.pos_x += CONF_EDITOR_CAM_SPEED * mv_horiz;
		g_cam.pos_y += CONF_EDITOR_CAM_SPEED * mv_vert;
	}
}

static void
draw_bg(void)
{
	static uint8_t cbg[] = CONF_COLOR_BG;
	SDL_SetRenderDrawColor(rend, cbg[0], cbg[1], cbg[2], 255);
	SDL_RenderClear(rend);
}

static void
draw_indicators(void)
{
	// draw hover / selection boundary.
	{
		static uint8_t cb[] = CONF_COLOR_EDITOR_BOUNDARY;
		
		SDL_SetRenderDrawColor(rend,
		                       cb[0],
		                       cb[1],
		                       cb[2],
		                       CONF_COLOR_EDITOR_BOUNDARY_OPACITY);
		
		switch (mode)
		{
		case EM_TILE:
		{
			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = (int)sel_x;
			sel_y = (int)sel_y;
			
			relative_draw_rect(rend, sel_x, sel_y, 1.0f, 1.0f);
			
			break;
		}
		case EM_TRIGGER:
			// TODO: implement.
			break;
		}
	}
	
	// draw current type indicator.
	{
		static uint8_t co[] = CONF_COLOR_OUTLINE;
		
		SDL_Rect r =
		{
			.x = CONF_WND_WIDTH - CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.y = 0,
			.w = CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.h = CONF_EDITOR_TYPE_INDICATOR_SIZE,
		};
		
		uint8_t const *col;
		switch (mode)
		{
		case EM_TILE:
			col = map_tile_color(type);
			break;
		case EM_TRIGGER:
			col = trigger_color(type);
			break;
		}
		
		SDL_SetRenderDrawColor(rend, col[0], col[1], col[2], 255);
		SDL_RenderFillRect(rend, &r);
		
		SDL_SetRenderDrawColor(rend, co[0], co[1], co[2], 255);
		SDL_RenderDrawRect(rend, &r);
	}
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
