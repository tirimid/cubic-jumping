#include "editor.h"

#include <stdlib.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "conf.h"
#include "input.h"
#include "map.h"
#include "text.h"
#include "triggers.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

#define NO_DRAG_REGION 0.0f

typedef enum edit_mode
{
	EM_TILE_P = 0,
	EM_TILE_F,
	EM_TRIGGER,
	EM_PLAYER,
} edit_mode;

static void update_editor(void);
static void draw_bg(void);
static void draw_indicators(void);
static void btn_mode_tile_p(void);
static void btn_mode_tile_f(void);
static void btn_mode_trigger(void);
static void btn_mode_player(void);
static void btn_type_next(void);
static void btn_type_prev(void);
static void btn_zoom_in(void);
static void btn_zoom_out(void);
static void btn_save(void);
static void btn_arg_add(void);
static void btn_arg_sub(void);
static void btn_single(void);
static void btn_exit(void);

static char const *map_file;
static edit_mode mode = EM_TILE_P;
static int type = 0;
static bool unsaved = false;
static float drag_orig_x = NO_DRAG_REGION, drag_orig_y = NO_DRAG_REGION;
static uint32_t arg = 0;
static bool single_use = true;
static bool running;

int
editor_init(char const *file)
{
	map_file = file;
	if (map_load_from_file(file))
		return 1;
	
	// init editor state.
	{
		mode = EM_TILE_P;
		type = 0;
		unsaved = false;
		drag_orig_x = drag_orig_y = NO_DRAG_REGION;
		arg = 0;
		single_use = true;
		running = true;
	}
	
	// init camera state.
	{
		g_cam.pos_x = 0.0f;
		g_cam.pos_y = 0.0f;
		g_cam.zoom = CONF_CAM_MAX_ZOOM;
	}
	
	return 0;
}

void
editor_loop(void)
{
	ui_button b_mode_tile_p = ui_button_create(10, 10, "Tile-P", btn_mode_tile_p);
	ui_button b_mode_tile_f = ui_button_create(160, 10, "Tile-F", btn_mode_tile_f);
	ui_button b_mode_trigger = ui_button_create(310, 10, "Trigger", btn_mode_trigger);
	ui_button b_mode_player = ui_button_create(480, 10, "Player", btn_mode_player);
	ui_button b_zoom_in = ui_button_create(10, 50, "Zoom+", btn_zoom_in);
	ui_button b_zoom_out = ui_button_create(135, 50, "Zoom-", btn_zoom_out);
	ui_button b_save = ui_button_create(260, 50, "Save", btn_save);
	ui_button b_arg_add = ui_button_create(365, 50, "Arg+", btn_arg_add);
	ui_button b_arg_sub = ui_button_create(470, 50, "Arg-", btn_arg_sub);
	ui_button b_single = ui_button_create(575, 50, "Single", btn_single);
	ui_button b_type_next = ui_button_create(320, 90, "Type>", btn_type_next);
	ui_button b_type_prev = ui_button_create(450, 90, "Type<", btn_type_prev);
	ui_button b_exit = ui_button_create(580, 90, "Exit", btn_exit);
	
	while (running)
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
		
		// update editor.
		{
			// update UI.
			{
				ui_button_update(&b_mode_tile_p);
				ui_button_update(&b_mode_tile_f);
				ui_button_update(&b_mode_trigger);
				ui_button_update(&b_mode_player);
				ui_button_update(&b_zoom_in);
				ui_button_update(&b_zoom_out);
				ui_button_update(&b_type_next);
				ui_button_update(&b_type_prev);
				ui_button_update(&b_save);
				ui_button_update(&b_arg_add);
				ui_button_update(&b_arg_sub);
				ui_button_update(&b_single);
				ui_button_update(&b_exit);
			}
			
			update_editor();
			keybd_post_update();
			mouse_post_update();
		}
		
		// draw editor.
		{
			draw_bg();
			map_draw();
			map_draw_outlines();
			triggers_draw();
			draw_indicators();
			
			// draw UI.
			{
				ui_button_draw(&b_mode_tile_p);
				ui_button_draw(&b_mode_tile_f);
				ui_button_draw(&b_mode_trigger);
				ui_button_draw(&b_mode_player);
				ui_button_draw(&b_zoom_in);
				ui_button_draw(&b_zoom_out);
				ui_button_draw(&b_type_next);
				ui_button_draw(&b_type_prev);
				ui_button_draw(&b_save);
				ui_button_draw(&b_arg_add);
				ui_button_draw(&b_arg_sub);
				ui_button_draw(&b_single);
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
update_editor(void)
{
	// move camera.
	{
		float mv_horiz = key_down(CONF_EDITOR_KEY_RIGHT) - key_down(CONF_EDITOR_KEY_LEFT);
		float mv_vert = key_down(CONF_EDITOR_KEY_DOWN) - key_down(CONF_EDITOR_KEY_UP);
		g_cam.pos_x += CONF_EDITOR_CAM_SPEED * mv_horiz;
		g_cam.pos_y += CONF_EDITOR_CAM_SPEED * mv_vert;
	}
	
	// mouse interaction based on mode.
	switch (mode)
	{
	case EM_TILE_P:
	{
		int mouse_x, mouse_y;
		mouse_pos(&mouse_x, &mouse_y);
		
		if (mouse_y < CONF_EDITOR_BAR_SIZE)
			break;
		
		if (mouse_down(MB_LEFT))
		{
			unsaved = true;
			
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = MAX(0.0f, sel_x);
			sel_y = MAX(0.0f, sel_y);
			
			if (sel_x >= g_map.size_x)
				map_grow((int)sel_x - g_map.size_x + 1, 0);
			if (sel_y >= g_map.size_y)
				map_grow(0, (int)sel_y - g_map.size_y + 1);
			
			map_get((int)sel_x, (int)sel_y)->type = type;
		}
		
		break;
	}
	case EM_TILE_F:
	{
		int mouse_x, mouse_y;
		mouse_pos(&mouse_x, &mouse_y);
		
		if (mouse_y < CONF_EDITOR_BAR_SIZE)
			break;
		
		float drag_x, drag_y;
		screen_to_game_coord(&drag_x, &drag_y, mouse_x, mouse_y);
		
		if (mouse_pressed(MB_LEFT))
		{
			drag_orig_x = drag_x;
			drag_orig_y = drag_y;
		}
		else if (mouse_released(MB_LEFT))
		{
			unsaved = true;
			
			if (drag_x < drag_orig_x)
			{
				float tmp = drag_x;
				drag_x = drag_orig_x;
				drag_orig_x = tmp;
			}
			
			if (drag_y < drag_orig_y)
			{
				float tmp = drag_y;
				drag_y = drag_orig_y;
				drag_orig_y = tmp;
			}
			
			drag_x = CLAMP(0.0f, drag_x, g_map.size_x - 1);
			drag_y = CLAMP(0.0f, drag_y, g_map.size_y - 1);
			drag_orig_x = CLAMP(0.0f, drag_orig_x, g_map.size_x - 1);
			drag_orig_y = CLAMP(0.0f, drag_orig_y, g_map.size_y - 1);
			
			for (int x = drag_orig_x; x < (int)drag_x + 1; ++x)
			{
				for (int y = drag_orig_y; y < (int)drag_y + 1; ++y)
					map_get(x, y)->type = type;
			}
			
			drag_orig_x = drag_orig_y = NO_DRAG_REGION;
		}
		
		break;
	}
	case EM_TRIGGER:
	{
		int mouse_x, mouse_y;
		mouse_pos(&mouse_x, &mouse_y);
		
		if (mouse_y < CONF_EDITOR_BAR_SIZE)
			break;
		
		float drag_x, drag_y;
		screen_to_game_coord(&drag_x, &drag_y, mouse_x, mouse_y);
		
		if (mouse_pressed(MB_LEFT))
		{
			drag_orig_x = drag_x;
			drag_orig_y = drag_y;
		}
		else if (mouse_released(MB_LEFT))
		{
			unsaved = true;
			
			if (drag_x < drag_orig_x)
			{
				float tmp = drag_x;
				drag_x = drag_orig_x;
				drag_orig_x = tmp;
			}
			
			if (drag_y < drag_orig_y)
			{
				float tmp = drag_y;
				drag_y = drag_orig_y;
				drag_orig_y = tmp;
			}
			
			trigger new_trigger =
			{
				.pos_x = drag_orig_x,
				.pos_y = drag_orig_y,
				.size_x = drag_x - drag_orig_x,
				.size_y = drag_y - drag_orig_y,
				.arg = arg,
				.single_use = single_use,
				.type = type,
			};
			triggers_add_trigger(&new_trigger);
			
			drag_orig_x = drag_orig_y = NO_DRAG_REGION;
		}
		
		if (mouse_down(MB_RIGHT))
		{
			for (size_t i = 0; i < g_ntriggers; ++i)
			{
				trigger const *trigger = &g_triggers[i];
				if (drag_x >= trigger->pos_x
				    && drag_x < trigger->pos_x + trigger->size_x
				    && drag_y >= trigger->pos_y
				    && drag_y < trigger->pos_y + trigger->size_y)
				{
					triggers_rm_trigger(i);
					unsaved = true;
				}
			}
		}
		
		break;
	}
	case EM_PLAYER:
	{
		int mouse_x, mouse_y;
		mouse_pos(&mouse_x, &mouse_y);
		
		if (mouse_y < CONF_EDITOR_BAR_SIZE)
			break;
		
		if (mouse_down(MB_LEFT))
		{
			unsaved = true;
			
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = MAX(0.0f, sel_x);
			sel_y = MAX(0.0f, sel_y);
			sel_x = (int)sel_x;
			sel_y = (int)sel_y;
			
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
	{
		static uint8_t cp[] = CONF_COLOR_PLAYER;
		
		SDL_SetRenderDrawColor(g_rend, cp[0], cp[1], cp[2], 255);
		
		relative_draw_rect(g_map.player_spawn_x,
		                   g_map.player_spawn_y,
		                   CONF_PLAYER_SIZE,
		                   CONF_PLAYER_SIZE);
	}
	
	// draw hover / selection boundary.
	{
		static uint8_t cb[] = CONF_COLOR_EDITOR_BOUNDARY;
		
		SDL_SetRenderDrawColor(g_rend,
		                       cb[0],
		                       cb[1],
		                       cb[2],
		                       CONF_COLOR_EDITOR_BOUNDARY_OPACITY);
		
		switch (mode)
		{
		case EM_TILE_P:
		case EM_PLAYER:
		{
			int mouse_x, mouse_y;
			mouse_pos(&mouse_x, &mouse_y);
			
			float sel_x, sel_y;
			screen_to_game_coord(&sel_x, &sel_y, mouse_x, mouse_y);
			sel_x = MAX(0.0f, sel_x);
			sel_y = MAX(0.0f, sel_y);
			sel_x = (int)sel_x;
			sel_y = (int)sel_y;
			
			relative_draw_rect(sel_x, sel_y, 1.0f, 1.0f);
			
			break;
		}
		case EM_TILE_F:
		case EM_TRIGGER:
		{
			if (drag_orig_x == NO_DRAG_REGION)
				break;
			
			int mouse_x, mouse_y;
			mouse_pos(&mouse_x, &mouse_y);
			
			float drag_x, drag_y;
			screen_to_game_coord(&drag_x, &drag_y, mouse_x, mouse_y);
			
			float lbx = MIN(drag_x, drag_orig_x);
			float lby = MIN(drag_y, drag_orig_y);
			float ubx = MAX(drag_x, drag_orig_x);
			float uby = MAX(drag_y, drag_orig_y);
			
			relative_draw_rect(lbx, lby, ubx - lbx, uby - lby);
			
			break;
		}
		}
	}
	
	// draw editor bar.
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
		
		switch (mode)
		{
		case EM_TILE_P:
		case EM_TILE_F:
		{
			uint8_t const *col = map_tile_color(type);
			SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
			SDL_RenderFillRect(g_rend, &r);
			break;
		}
		case EM_TRIGGER:
		{
			uint8_t const *col = trigger_color(type);
			SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
			SDL_RenderFillRect(g_rend, &r);
			break;
		}
		default:
			break;
		}
		
		SDL_SetRenderDrawColor(g_rend, co[0], co[1], co[2], 255);
		SDL_RenderDrawRect(g_rend, &r);
	}
	
	// draw arg indicator.
	{
		static char buf[32];
		snprintf(buf, sizeof(buf), "%08x", arg);
		text_draw_str(buf, 10, 100);
	}
	
	// draw single use indicator.
	{
		text_draw_str(single_use ? "Sing." : "Mult.", 200, 100);
	}
	
	// draw save status indicator.
	{
		static uint8_t ces[] = CONF_COLOR_EDITOR_SAVED;
		static uint8_t ceu[] = CONF_COLOR_EDITOR_UNSAVED;
		
		if (unsaved)
			SDL_SetRenderDrawColor(g_rend, ceu[0], ceu[1], ceu[2], 255);
		else
			SDL_SetRenderDrawColor(g_rend, ces[0], ces[1], ces[2], 255);
		
		SDL_Rect r =
		{
			.x = 0,
			.y = CONF_WND_HEIGHT - CONF_EDITOR_SAVE_INDICATOR_SIZE,
			.w = CONF_WND_WIDTH,
			.h = CONF_EDITOR_SAVE_INDICATOR_SIZE,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
}

static void
btn_mode_tile_p(void)
{
	if (mode != EM_TILE_P && mode != EM_TILE_F)
		type = 0;
	mode = EM_TILE_P;
}

static void
btn_mode_tile_f(void)
{
	if (mode != EM_TILE_P && mode != EM_TILE_F)
		type = 0;
	mode = EM_TILE_F;
}

static void
btn_mode_trigger(void)
{
	type = 0;
	mode = EM_TRIGGER;
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
	case EM_TILE_P:
	case EM_TILE_F:
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
	case EM_TILE_P:
	case EM_TILE_F:
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

static void
btn_save(void)
{
	if (!unsaved)
		return;
	
	map_refit_bounds();
	map_write_to_file(map_file);
	unsaved = false;
}

static void
btn_arg_add(void)
{
	++arg;
}

static void
btn_arg_sub(void)
{
	--arg;
}

static void
btn_single(void)
{
	single_use = !single_use;
}

static void
btn_exit(void)
{
	running = false;
}
