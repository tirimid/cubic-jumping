#include "player.h"

#include <stddef.h>

#include "cam.h"
#include "conf.h"
#include "game.h"
#include "keybd.h"
#include "map.h"
#include "map_list.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

#define COL_THRESHOLD 0.05f

player_t g_player;
player_state_t g_player_state;

static void update_playing(void);
static void update_dead(void);
static void collide(map_tile_t *tile);
static void collide_left(void);
static void collide_right(void);
static void collide_bottom(void);
static void collide_top(void);

void
player_update(void)
{
	static void (*update_fn[])(void) =
	{
		update_playing,
		update_dead,
	};
	
	update_fn[g_player_state]();
}

void
player_draw(void)
{
	if (g_player_state == PS_DEAD)
		return;
	
	static uint8_t cp[] = CONF_COLOR_PLAYER;
	SDL_SetRenderDrawColor(g_rend, cp[0], cp[1], cp[2], 255);
	relative_draw_rect(g_player.pos_x,
	                   g_player.pos_y,
	                   CONF_PLAYER_SIZE,
	                   CONF_PLAYER_SIZE);
}

bool
player_grounded(void)
{
	return g_player.dist_bottom < COL_THRESHOLD;
}

bool
player_walled_left(void)
{
	return g_player.dist_left < COL_THRESHOLD;
}

bool
player_walled_right(void)
{
	return g_player.dist_right < COL_THRESHOLD;
}

void
player_die(void)
{
	g_player.dead_ticks = CONF_PLAYER_DEATH_TICKS;
	g_player_state = PS_DEAD;
	
	for (int i = 0; i < CONF_PLAYER_SHARD_CNT; ++i)
	{
		vfx_put_particle(PT_PLAYER_SHARD,
		                 g_player.pos_x + CONF_PLAYER_SIZE / 2.0f,
		                 g_player.pos_y + CONF_PLAYER_SIZE / 2.0f);
	}
}

static void
update_playing(void)
{
	// rectify player bounds in map.
	do
	{
		if (g_player.pos_x < 0.0f)
			g_player.pos_x = 0.0f;
		if (g_player.pos_y < 0.0f)
			g_player.pos_y = 0.0f;
		
		if (g_player.pos_x > g_map.size_x - CONF_PLAYER_SIZE)
			g_player.pos_x = g_map.size_x - CONF_PLAYER_SIZE;
		if (g_player.pos_y > g_map.size_y - CONF_PLAYER_SIZE)
			g_player.pos_y = g_map.size_y - CONF_PLAYER_SIZE;
	} while (0);
	
	// find nearest left edge.
	do
	{
		int cxtl;
		map_tile_t *cxtl_tile = NULL;
		for (cxtl = g_player.pos_x; cxtl >= 0; --cxtl)
		{
			map_tile_t *tile = map_get(cxtl, g_player.pos_y);
			if (map_tile_collision(tile->type))
			{
				cxtl_tile = tile;
				break;
			}
		}
		
		int cxbl;
		map_tile_t *cxbl_tile = NULL;
		for (cxbl = g_player.pos_x; cxbl >= 0; --cxbl)
		{
			map_tile_t *tile = map_get(cxbl, g_player.pos_y + CONF_PLAYER_SIZE);
			if (map_tile_collision(tile->type))
			{
				cxbl_tile = tile;
				break;
			}
		}
		
		int cxl = MAX(cxtl, cxbl);
		g_player.dist_left = g_player.pos_x - cxl - 1.0f;
		g_player.near_left = cxtl > cxbl ? cxtl_tile : cxbl_tile;
	} while (0);
	
	// find nearest right edge.
	do
	{
		int cxtr;
		map_tile_t *cxtr_tile = NULL;
		for (cxtr = g_player.pos_x + CONF_PLAYER_SIZE; cxtr < g_map.size_x; ++cxtr)
		{
			map_tile_t *tile = map_get(cxtr, g_player.pos_y);
			if (map_tile_collision(tile->type))
			{
				cxtr_tile = tile;
				break;
			}
		}
		
		int cxbr;
		map_tile_t *cxbr_tile = NULL;
		for (cxbr = g_player.pos_x + CONF_PLAYER_SIZE; cxbr < g_map.size_x; ++cxbr)
		{
			map_tile_t *tile = map_get(cxbr, g_player.pos_y + CONF_PLAYER_SIZE);
			if (map_tile_collision(tile->type))
			{
				cxbr_tile = tile;
				break;
			}
		}
		
		int cxr = MIN(cxtr, cxbr);
		g_player.dist_right = cxr - g_player.pos_x - CONF_PLAYER_SIZE;
		g_player.near_right = cxtr < cxbr ? cxtr_tile : cxbr_tile;
	} while (0);
	
	// find nearest top edge.
	do
	{
		int cytl;
		map_tile_t *cytl_tile = NULL;
		for (cytl = g_player.pos_y; cytl >= 0; --cytl)
		{
			map_tile_t *tile = map_get(g_player.pos_x, cytl);
			if (map_tile_collision(tile->type))
			{
				cytl_tile = tile;
				break;
			}
		}
		
		int cytr;
		map_tile_t *cytr_tile = NULL;
		for (cytr = g_player.pos_y; cytr >= 0; --cytr)
		{
			map_tile_t *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cytr);
			if (map_tile_collision(tile->type))
			{
				cytr_tile = tile;
				break;
			}
		}
		
		int cyt = MAX(cytl, cytr);
		g_player.dist_top = g_player.pos_y - cyt - 1.0f;
		g_player.near_top = cytl > cytr ? cytl_tile : cytr_tile;
	} while (0);
	
	// find nearest bottom edge.
	do
	{
		int cybl;
		map_tile_t *cybl_tile = NULL;
		for (cybl = g_player.pos_y + CONF_PLAYER_SIZE; cybl < g_map.size_y; ++cybl)
		{
			map_tile_t *tile = map_get(g_player.pos_x, cybl);
			if (map_tile_collision(tile->type))
			{
				cybl_tile = tile;
				break;
			}
		}
		
		int cybr;
		map_tile_t *cybr_tile = NULL;
		for (cybr = g_player.pos_y + CONF_PLAYER_SIZE; cybr < g_map.size_y; ++cybr)
		{
			map_tile_t *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cybr);
			if (map_tile_collision(tile->type))
			{
				cybr_tile = tile;
				break;
			}
		}
		
		int cyb = MIN(cybl, cybr);
		g_player.dist_bottom = cyb - g_player.pos_y - CONF_PLAYER_SIZE;
		g_player.near_bottom = cybl < cybr ? cybl_tile : cybr_tile;
	} while (0);
	
	// apply user movement input velocity.
	do
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
	} while (0);
	
	// apply environmental forces.
	do
	{
		g_player.vel_y += CONF_GRAVITY;
	} while (0);
	
	// apply collision corrections.
	do
	{
		if (g_player.vel_x < 0.0f
		    && -g_player.vel_x >= g_player.dist_left
		    && g_player.dist_left < g_player.dist_right)
		{
			collide_left();
			collide(g_player.near_left);
		}
		
		if (g_player.vel_x > 0.0f
		    && g_player.vel_x >= g_player.dist_right
		    && g_player.dist_right < g_player.dist_left)
		{
			collide_right();
			collide(g_player.near_right);
		}
		
		if (g_player.vel_y < 0.0f
		    && -g_player.vel_y >= g_player.dist_top
		    && g_player.dist_top < g_player.dist_bottom)
		{
			collide_top();
			collide(g_player.near_top);
		}
		
		if (g_player.vel_y > 0.0f
		    && g_player.vel_y >= g_player.dist_bottom
		    && g_player.dist_bottom < g_player.dist_top)
		{
			collide_bottom();
			collide(g_player.near_bottom);
		}
	} while (0);
	
	// actually move player.
	do
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
	} while (0);
	
	// advance trace spawn counter, and spawn if needed.
	do
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
	} while (0);
}

static void
update_dead(void)
{
	if (g_player.dead_ticks == 0)
	{
		// map list reload simulates a respawn procedure.
		map_list_reload();
		return;
	}
	
	--g_player.dead_ticks;
}

static void
collide(map_tile_t *tile)
{
	// implement behavior that doesn't depend on collision direction.
	switch (tile->type)
	{
	case MTT_KILL:
		player_die();
		break;
	case MTT_END:
		if (g_game.off_switches == 0)
			map_list_load_next();
		break;
	case MTT_SWITCH_OFF:
		tile->type = MTT_SWITCH_ON;
		--g_game.off_switches;
		break;
	default:
		break;
	}
}

static void
collide_left(void)
{
	if (!g_player.near_left)
		return;
	
	g_player.pos_x -= g_player.dist_left - 0.001f;
	
	switch (g_player.near_left->type)
	{
	case MTT_BOUNCE:
		g_player.vel_x *= -CONF_RESTITUTION;
		break;
	case MTT_LAUNCH:
		g_player.vel_x = CONF_LAUNCH_FORCE_X;
		g_player.vel_y = -CONF_LAUNCH_FORCE_Y;
		break;
	default:
		g_player.vel_x = 0.0f;
		break;
	}
}

static void
collide_right(void)
{
	if (!g_player.near_right)
		return;
	
	g_player.pos_x += g_player.dist_right - 0.001f;
	
	switch (g_player.near_right->type)
	{
	case MTT_BOUNCE:
		g_player.vel_x *= -CONF_RESTITUTION;
		break;
	case MTT_LAUNCH:
		g_player.vel_x = -CONF_LAUNCH_FORCE_X;
		g_player.vel_y = -CONF_LAUNCH_FORCE_Y;
		break;
	default:
		g_player.vel_x = 0.0f;
		break;
	}
}

static void
collide_bottom(void)
{
	if (!g_player.near_bottom)
		return;
	
	g_player.pos_y += g_player.dist_bottom - 0.001f;
	
	switch (g_player.near_bottom->type)
	{
	case MTT_BOUNCE:
		g_player.vel_y *= -CONF_RESTITUTION;
		break;
	case MTT_LAUNCH:
		g_player.vel_y = -CONF_LAUNCH_FORCE_Y;
		break;
	default:
		g_player.vel_y = 0.0f;
		break;
	}
}

static void
collide_top(void)
{
	if (!g_player.near_top)
		return;
	
	g_player.pos_y -= g_player.dist_top - 0.001f;
	
	switch (g_player.near_top->type)
	{
	case MTT_BOUNCE:
		g_player.vel_y *= -CONF_RESTITUTION;
		break;
	default:
		g_player.vel_y = 0.0f;
		break;
	}
}
