#include "editor.h"

#include <stdbool.h>
#include <stdlib.h>

#include <SDL.h>

#include "cam.h"
#include "conf.h"
#include "keybd.h"
#include "map.h"
#include "util.h"

static void draw_bg(void);
static void draw_editor_state(void);

static SDL_Window *wnd;
static SDL_Renderer *rend;
static map_tile_type_t cur_type = MTT_GROUND;
static char const *map_file, *map_name;
static bool unsaved = false;

int
editor_init(char const *file, char const *name)
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
	
	if (map_load_from_file(file))
		return 1;
	
	map_file = file;
	map_name = name;
	
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
editor_update(void)
{
	if (key_down(K_PRESS) && key_down(K_POWERJUMP))
	{
		if (key_pressed(K_JUMP)
		    && g_cam.pos_x >= 0.0f
		    && g_cam.pos_y >= 0.0f)
		{
			g_map.player_spawn_x = g_cam.pos_x;
			g_map.player_spawn_y = g_cam.pos_y;
		}
	}
	else if (key_down(K_PRESS))
	{
		float zoom_dir = key_down(K_JUMP) - key_down(K_FALL);
		g_cam.zoom += CONF_CAM_ZOOM_SPEED * zoom_dir;
		
		if (key_pressed(K_LEFT))
		{
			++cur_type;
			cur_type %= MTT_END__;
		}
		else if (key_pressed(K_RIGHT))
		{
			map_write_to_file(map_file, map_name);
			unsaved = false;
		}
	}
	else if (key_down(K_POWERJUMP))
	{
		float mv_horiz = key_down(K_RIGHT) - key_down(K_LEFT);
		float mv_vert = key_down(K_FALL) - key_down(K_JUMP);
		g_cam.pos_x += 0.1f * mv_horiz;
		g_cam.pos_y += 0.1f * mv_vert;
		
		if (g_cam.pos_x < 0.0f || g_cam.pos_y < 0.0f)
			return;
		
		if ((int)g_cam.pos_x >= g_map.size_x)
			map_grow((int)g_cam.pos_x - g_map.size_x + 1, 0);
		if ((int)g_cam.pos_y >= g_map.size_y)
			map_grow(0, (int)g_cam.pos_y - g_map.size_y + 1);
		
		map_tile_t *tile = map_get((int)g_cam.pos_x, (int)g_cam.pos_y);
		*tile = (map_tile_t)
		{
			.type = cur_type,
		};
		unsaved = true;
	}
	else
	{
		float mv_horiz = key_down(K_RIGHT) - key_down(K_LEFT);
		float mv_vert = key_down(K_FALL) - key_down(K_JUMP);
		g_cam.pos_x += 0.1f * mv_horiz;
		g_cam.pos_y += 0.1f * mv_vert;
	}
	
	g_cam.zoom = MAX(g_cam.zoom, 0.25f);
	g_cam.zoom = MIN(g_cam.zoom, 1.0f);
}

void
editor_main_loop(void)
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
		
		// update editor.
		editor_update();
		keybd_post_update();
		
		// draw editor.
		draw_bg();
		map_draw(rend);
		draw_editor_state();
		SDL_RenderPresent(rend);
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
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
draw_editor_state(void)
{
	// draw player spawn point.
	{
		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		relative_draw_rect(rend,
		                   g_map.player_spawn_x,
		                   g_map.player_spawn_y,
		                   1.0f,
		                   1.0f);
	}
	
	// draw current highlight indicator.
	{
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		relative_draw_rect(rend,
		                   (int)g_cam.pos_x,
		                   (int)g_cam.pos_y,
		                   1.0f,
		                   1.0f);
	}
	
	// draw unsaved progress indicator.
	{
		if (unsaved)
			SDL_SetRenderDrawColor(rend, 200, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(rend, 0, 200, 0, 255);
		
		SDL_Rect rect =
		{
			.x = 0,
			.y = CONF_WND_HEIGHT - 10,
			.w = CONF_WND_WIDTH,
			.h = 10,
		};
		SDL_RenderFillRect(rend, &rect);
	}
	
	// draw current tile type.
	{
		if (cur_type == MTT_AIR)
			return;
		
		float const *ct = map_tile_color(cur_type);
		SDL_SetRenderDrawColor(rend, ct[0], ct[1], ct[2], 255);
		
		SDL_Rect rect =
		{
			.x = 0,
			.y = 0,
			.w = 70,
			.h = 70,
		};
		SDL_RenderFillRect(rend, &rect);
	}
}
