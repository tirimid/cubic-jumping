#include "player.h"

#include "cam.h"
#include "conf.h"
#include "keybd.h"
#include "map.h"
#include "util.h"
#include "vfx.h"

player_t g_player;

void
player_update(void)
{
	// rectify player bounds in map.
	{
		if (g_player.pos_x < 0.0f)
			g_player.pos_x = 0.0f;
		if (g_player.pos_y < 0.0f)
			g_player.pos_y = 0.0f;
		
		if (g_player.pos_x > g_map.size_x - CONF_PLAYER_SIZE)
			g_player.pos_x = g_map.size_x - CONF_PLAYER_SIZE;
		if (g_player.pos_y > g_map.size_y - CONF_PLAYER_SIZE)
			g_player.pos_y = g_map.size_y - CONF_PLAYER_SIZE;
	}
	
	// compute distance to nearest left edge.
	{
		int cxtl;
		for (cxtl = g_player.pos_x; cxtl > 0; --cxtl)
		{
			map_tile_t *tile = map_get(cxtl, g_player.pos_y);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cxbl;
		for (cxbl = g_player.pos_x; cxbl > 0; --cxbl)
		{
			map_tile_t *tile = map_get(cxbl, g_player.pos_y + CONF_PLAYER_SIZE);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cxl = MAX(cxtl, cxbl);
		g_player.dist_left = g_player.pos_x - cxl - 1.0f;
	}
	
	// compute distance to nearest right edge.
	{
		int cxtr;
		for (cxtr = g_player.pos_x + CONF_PLAYER_SIZE; cxtr < g_map.size_x; ++cxtr)
		{
			map_tile_t *tile = map_get(cxtr, g_player.pos_y);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cxbr;
		for (cxbr = g_player.pos_x + CONF_PLAYER_SIZE; cxbr < g_map.size_x; ++cxbr)
		{
			map_tile_t *tile = map_get(cxbr, g_player.pos_y + CONF_PLAYER_SIZE);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cxr = MIN(cxtr, cxbr);
		g_player.dist_right = cxr - g_player.pos_x - CONF_PLAYER_SIZE;
	}
	
	// compute distance to nearest top edge.
	{
		int cytl;
		for (cytl = g_player.pos_y; cytl > 0; --cytl)
		{
			map_tile_t *tile = map_get(g_player.pos_x, cytl);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cytr;
		for (cytr = g_player.pos_y; cytr > 0; --cytr)
		{
			map_tile_t *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cytr);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cyt = MAX(cytl, cytr);
		g_player.dist_top = g_player.pos_y - cyt - 1.0f;
	}
	
	// compute distance to nearest bottom edge.
	{
		int cybl;
		for (cybl = g_player.pos_y + CONF_PLAYER_SIZE; cybl < g_map.size_y; ++cybl)
		{
			map_tile_t *tile = map_get(g_player.pos_x, cybl);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cybr;
		for (cybr = g_player.pos_y + CONF_PLAYER_SIZE; cybr < g_map.size_y; ++cybr)
		{
			map_tile_t *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cybr);
			if (tile->type != MTT_AIR)
				break;
		}
		
		int cyb = MIN(cybl, cybr);
		g_player.dist_bottom = cyb - g_player.pos_y - CONF_PLAYER_SIZE;
	}
	
	// apply user movement input velocity.
	{
		float mv_horiz = key_down(K_RIGHT) - key_down(K_LEFT);
		mv_horiz *= player_grounded() ? CONF_PLAYER_SPEED : CONF_PLAYER_AIR_SPEED;
		g_player.vel_x += mv_horiz;
		
		if (key_down(K_FALL))
			g_player.vel_y += CONF_PLAYER_FALL_ACCEL;
		
		if (player_grounded() && key_down(K_JUMP))
			g_player.vel_y = -CONF_PLAYER_JUMP_FORCE;
		else if (player_grounded() && key_down(K_POWERJUMP))
		{
			if (g_player.vel_x > 0.0f)
				g_player.vel_x = CONF_PLAYER_POWERJUMP_FORCE_X;
			else if (g_player.vel_x < 0.0f)
				g_player.vel_x = -CONF_PLAYER_POWERJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_POWERJUMP_FORCE_Y;
		}
		
		if (player_walled_left() && key_down(K_JUMP))
		{
			g_player.vel_x = CONF_PLAYER_WALLJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_WALLJUMP_FORCE_Y;
		}
		
		if (player_walled_right() && key_down(K_JUMP))
		{
			g_player.vel_x = -CONF_PLAYER_WALLJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_WALLJUMP_FORCE_Y;
		}
	}
	
	// apply environmental forces.
	{
		g_player.vel_y += CONF_GRAVITY;
	}
	
	// apply collision corrections.
	{
		if (g_player.vel_x < 0.0f
		    && -g_player.vel_x >= g_player.dist_left
		    && g_player.dist_left < g_player.dist_right)
		{
			g_player.pos_x -= g_player.dist_left - 0.001f;
			g_player.vel_x = 0.0f;
		}
		
		if (g_player.vel_x > 0.0f
		    && g_player.vel_x >= g_player.dist_right
		    && g_player.dist_right < g_player.dist_left)
		{
			g_player.pos_x += g_player.dist_right - 0.001f;
			g_player.vel_x = 0.0f;
		}
		
		if (g_player.vel_y < 0.0f
		    && -g_player.vel_y >= g_player.dist_top
		    && g_player.dist_top < g_player.dist_bottom)
		{
			g_player.pos_y -= g_player.dist_top - 0.001f;
			g_player.vel_y = 0.0f;
		}
		
		if (g_player.vel_y > 0.0f
		    && g_player.vel_y >= g_player.dist_bottom
		    && g_player.dist_bottom < g_player.dist_top)
		{
			g_player.pos_y += g_player.dist_bottom - 0.001f;
			g_player.vel_y = 0.0f;
		}
	}
	
	// actually move player.
	{
		g_player.pos_x += g_player.vel_x;
		g_player.pos_y += g_player.vel_y;
		
		g_player.vel_x /= player_grounded() ? CONF_FRICTION : CONF_DRAG;
		g_player.vel_y /= CONF_DRAG;
		
		if (player_walled_left() && key_down(K_LEFT)
		    || player_walled_right() && key_down(K_RIGHT))
		{
			g_player.vel_y /= CONF_WALL_SLIDE_FRICTION;
		}
	}
	
	// advance trace spawn counter, and spawn if needed.
	{
		if (g_player.trace_spawn_ticks == 0)
		{
			vfx_put_particle(PT_PLAYER_TRACE,
			                 g_player.pos_x + CONF_PLAYER_SIZE / 2.0f,
			                 g_player.pos_y + CONF_PLAYER_SIZE / 2.0f);
			g_player.trace_spawn_ticks = CONF_PLAYER_TRACE_SPAWN_TICKS;
		}
		else
			--g_player.trace_spawn_ticks;
	}
}

void
player_draw(SDL_Renderer *rend)
{
	static uint8_t cp[] = CONF_COLOR_PLAYER;
	SDL_SetRenderDrawColor(rend, cp[0], cp[1], cp[2], 255);
	
	relative_draw_rect(rend,
	                   g_player.pos_x,
	                   g_player.pos_y,
	                   CONF_PLAYER_SIZE,
	                   CONF_PLAYER_SIZE);
}

bool
player_grounded(void)
{
	return g_player.dist_bottom < CONF_GROUND_THRESHOLD;
}

bool
player_walled_left(void)
{
	return g_player.dist_left < CONF_GROUND_THRESHOLD;
}

bool
player_walled_right(void)
{
	return g_player.dist_right < CONF_GROUND_THRESHOLD;
}
