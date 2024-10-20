#include "player.h"

#include <stddef.h>

#include "cam.h"
#include "conf.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "options.h"
#include "sound.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

#define COL_THRESHOLD 0.05f

struct player g_player;
enum player_state g_player_state;
struct player_cap_mask g_player_cap_mask;

static void update_playing(void);
static void update_dead(void);
static void collide(struct map_tile *tile);
static void collide_left(void);
static void collide_right(void);
static void collide_bottom(void);
static void collide_top(void);
static void compute_collision_distances(void);
static void test_and_apply_collisions(void);

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
	return g_player.dist_bottom < COL_THRESHOLD && !g_player.short_circuit;
}

bool
player_walled_left(void)
{
	return g_player.dist_left < COL_THRESHOLD && !g_player.short_circuit;
}

bool
player_walled_right(void)
{
	return g_player.dist_right < COL_THRESHOLD && !g_player.short_circuit;
}

void
player_die(void)
{
	// prevent "double-death", incrementing game deaths by 2 instead of 1.
	if (g_player_state == PS_DEAD)
		return;
	
	++g_game.il_deaths;
	++g_game.total_deaths;
	
	g_player.dead_ticks = CONF_PLAYER_DEATH_TICKS;
	g_player_state = PS_DEAD;
	
	for (int i = 0; i < CONF_PLAYER_SHARD_CNT; ++i)
	{
		vfx_put_particle(PT_PLAYER_SHARD,
		                 g_player.pos_x + CONF_PLAYER_SIZE / 2.0f,
		                 g_player.pos_y + CONF_PLAYER_SIZE / 2.0f);
	}
	
	sound_play_sfx(SI_DEATH);
}

void
player_set_cap_mask(enum player_cap_id id, bool state)
{
	switch (id)
	{
	case PCI_JUMP:
		g_player_cap_mask.no_jump = state;
		break;
	case PCI_WALLJUMP:
		g_player_cap_mask.no_walljump = state;
		break;
	case PCI_WALLSLIDE:
		g_player_cap_mask.no_wallslide = state;
		break;
	case PCI_POWERJUMP:
		g_player_cap_mask.no_powerjump = state;
		break;
	case PCI_DASH_DOWN:
		g_player_cap_mask.no_dash_down = state;
		break;
	}
}

static void
update_playing(void)
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
	
	// need to initially check collisions prior to player movement being
	// applied in order to fix bug when you can jump on kill blocks.
	{
		// the short circuit mechanism exists as a way to allow certain
		// collision tiles to demand exclusive collision handling instead of
		// the others.
		// for example, the level end tiles need to short circuit, as
		// otherwise the player landing on a level end tile could cause the
		// level end menu to open multiple times in a row as a result of
		// collision being tested twice during a player cycle.
		g_player.short_circuit = false;
		
		test_and_apply_collisions();
	}
	
	// apply user movement input velocity.
	{
		if (player_grounded() || (!player_grounded() && g_player.air_control))
		{
			float mv_horiz = key_down(g_options.k_right) - key_down(g_options.k_left);
			mv_horiz *= player_grounded() ? CONF_PLAYER_SPEED : CONF_PLAYER_AIR_SPEED;
			g_player.vel_x += mv_horiz;
		}
		
		if (!g_player_cap_mask.no_dash_down
		    && !player_grounded()
		    && key_pressed(g_options.k_dash_down))
		{
			g_player.vel_y = CONF_PLAYER_DASH_DOWN_SPEED;
			for (int i = 0; i < CONF_AIR_PUFF_CNT; ++i)
			{
				vfx_put_particle(PT_AIR_PUFF,
				                 g_player.pos_x + CONF_PLAYER_SIZE / 2.0f,
				                 g_player.pos_y + CONF_PLAYER_SIZE / 2.0f);
			}
			sound_play_sfx(SI_DASH_DOWN);
		}
		
		if (!g_player_cap_mask.no_jump
		    && player_grounded()
		    && key_down(g_options.k_jump))
		{
			g_player.vel_y = -CONF_PLAYER_JUMP_FORCE;
			sound_play_sfx(SI_JUMP);
		}
		else if (!g_player_cap_mask.no_powerjump
		         && player_grounded()
		         && key_down(g_options.k_powerjump))
		{
			if (g_player.vel_x > 0.0f)
				g_player.vel_x = CONF_PLAYER_POWERJUMP_FORCE_X;
			else if (g_player.vel_x < 0.0f)
				g_player.vel_x = -CONF_PLAYER_POWERJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_POWERJUMP_FORCE_Y;
			sound_play_sfx(SI_POWERJUMP);
		}
		
		if (!g_player_cap_mask.no_wallslide
		    && player_walled_left()
		    && key_down(g_options.k_left)
		    && !map_tile_slippery(g_player.near_left->type))
		{
			if (map_tile_climbable(g_player.near_left->type))
				g_player.vel_y = -CONF_CLIMB_SPEED;
			else
				g_player.vel_y /= CONF_WALL_SLIDE_FRICTION;
		}
		
		if (!g_player_cap_mask.no_wallslide
		    && player_walled_right()
		    && key_down(g_options.k_right)
		    && !map_tile_slippery(g_player.near_right->type))
		{
			if (map_tile_climbable(g_player.near_right->type))
				g_player.vel_y = -CONF_CLIMB_SPEED;
			else
				g_player.vel_y /= CONF_WALL_SLIDE_FRICTION;
		}
		
		if (!g_player_cap_mask.no_walljump
		    && player_walled_left()
		    && key_down(g_options.k_jump)
		    && !map_tile_slippery(g_player.near_left->type))
		{
			g_player.vel_x = CONF_PLAYER_WALLJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_WALLJUMP_FORCE_Y;
			sound_play_sfx(SI_WALLJUMP);
		}
		
		if (!g_player_cap_mask.no_walljump
		    && player_walled_right()
		    && key_down(g_options.k_jump)
		    && !map_tile_slippery(g_player.near_right->type))
		{
			g_player.vel_x = -CONF_PLAYER_WALLJUMP_FORCE_X;
			g_player.vel_y = -CONF_PLAYER_WALLJUMP_FORCE_Y;
			sound_play_sfx(SI_WALLJUMP);
		}
	}
	
	// apply environmental forces.
	{
		g_player.vel_y += CONF_GRAVITY;
		g_player.vel_x /= player_grounded() ? CONF_FRICTION : CONF_DRAG;
		g_player.vel_y /= CONF_DRAG;
	}
	
	// need to also apply collsions after all velocity changes.
	{
		test_and_apply_collisions();
	}
	
	// actually move player.
	{
		g_player.pos_x += g_player.vel_x;
		g_player.pos_y += g_player.vel_y;
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

static void
update_dead(void)
{
	if (g_player.dead_ticks == 0)
	{
		// map list reload simulates a respawn procedure.
		map_list_soft_reload();
		return;
	}
	
	--g_player.dead_ticks;
}

static void
collide(struct map_tile *tile)
{
	if (!tile)
		return;
	
	g_player.air_control = true;
	
	// implement behavior that doesn't depend on collision direction.
	switch (tile->type)
	{
	case MTT_KILL:
		g_player.short_circuit = true;
		player_die();
		break;
	case MTT_LAUNCH:
		sound_play_sfx(SI_LAUNCH);
		break;
	case MTT_END_ON:
		g_player.short_circuit = true;
		sound_play_sfx(SI_END);
		map_list_load_next();
		break;
	case MTT_SWITCH_OFF:
		tile->type = MTT_SWITCH_ON;
		sound_play_sfx(SI_SWITCH);
		game_enable_switch();
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
		if (g_player.vel_x < -CONF_MIN_RESTITUTION_SPEED)
		{
			g_player.vel_x *= -CONF_RESTITUTION;
			sound_play_sfx(SI_BOUNCE);
		}
		else
			g_player.vel_x = 0.0f;
		break;
	case MTT_LAUNCH:
		g_player.air_control = false;
		g_player.short_circuit = true;
		g_player.vel_x = CONF_WALL_LAUNCH_FORCE_X;
		g_player.vel_y = -CONF_WALL_LAUNCH_FORCE_Y;
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
		if (g_player.vel_x > CONF_MIN_RESTITUTION_SPEED)
		{
			g_player.vel_x *= -CONF_RESTITUTION;
			sound_play_sfx(SI_BOUNCE);
		}
		else
			g_player.vel_x = 0.0f;
		break;
	case MTT_LAUNCH:
		g_player.air_control = false;
		g_player.short_circuit = true;
		g_player.vel_x = -CONF_WALL_LAUNCH_FORCE_X;
		g_player.vel_y = -CONF_WALL_LAUNCH_FORCE_Y;
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
		if (g_player.vel_y > CONF_MIN_RESTITUTION_SPEED)
		{
			g_player.vel_y *= -CONF_RESTITUTION;
			sound_play_sfx(SI_BOUNCE);
		}
		else
			g_player.vel_y = 0.0f;
		break;
	case MTT_LAUNCH:
		g_player.vel_y = -CONF_LAUNCH_FORCE;
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
		if (g_player.vel_y < -CONF_MIN_RESTITUTION_SPEED)
		{
			g_player.vel_y *= -CONF_RESTITUTION;
			sound_play_sfx(SI_BOUNCE);
		}
		else
			g_player.vel_y = 0.0f;
		break;
	default:
		g_player.vel_y = 0.0f;
		break;
	}
}

static void
compute_collision_distances(void)
{
	// find nearest left edge.
	{
		int cxtl;
		struct map_tile *cxtl_tile = NULL;
		for (cxtl = g_player.pos_x; cxtl >= 0; --cxtl)
		{
			struct map_tile *tile = map_get(cxtl, g_player.pos_y);
			if (map_tile_collision(tile->type))
			{
				cxtl_tile = tile;
				break;
			}
		}
		
		int cxbl;
		struct map_tile *cxbl_tile = NULL;
		for (cxbl = g_player.pos_x; cxbl >= 0; --cxbl)
		{
			struct map_tile *tile = map_get(cxbl, g_player.pos_y + CONF_PLAYER_SIZE);
			if (map_tile_collision(tile->type))
			{
				cxbl_tile = tile;
				break;
			}
		}
		
		int cxl = MAX(cxtl, cxbl);
		g_player.dist_left = g_player.pos_x - cxl - 1.0f;
		g_player.near_left = cxtl > cxbl ? cxtl_tile : cxbl_tile;
	}
	
	// find nearest right edge.
	{
		int cxtr;
		struct map_tile *cxtr_tile = NULL;
		for (cxtr = g_player.pos_x + CONF_PLAYER_SIZE; cxtr < g_map.size_x; ++cxtr)
		{
			struct map_tile *tile = map_get(cxtr, g_player.pos_y);
			if (map_tile_collision(tile->type))
			{
				cxtr_tile = tile;
				break;
			}
		}
		
		int cxbr;
		struct map_tile *cxbr_tile = NULL;
		for (cxbr = g_player.pos_x + CONF_PLAYER_SIZE; cxbr < g_map.size_x; ++cxbr)
		{
			struct map_tile *tile = map_get(cxbr, g_player.pos_y + CONF_PLAYER_SIZE);
			if (map_tile_collision(tile->type))
			{
				cxbr_tile = tile;
				break;
			}
		}
		
		int cxr = MIN(cxtr, cxbr);
		g_player.dist_right = cxr - g_player.pos_x - CONF_PLAYER_SIZE;
		g_player.near_right = cxtr < cxbr ? cxtr_tile : cxbr_tile;
	}
	
	// find nearest top edge.
	{
		int cytl;
		struct map_tile *cytl_tile = NULL;
		for (cytl = g_player.pos_y; cytl >= 0; --cytl)
		{
			struct map_tile *tile = map_get(g_player.pos_x, cytl);
			if (map_tile_collision(tile->type))
			{
				cytl_tile = tile;
				break;
			}
		}
		
		int cytr;
		struct map_tile *cytr_tile = NULL;
		for (cytr = g_player.pos_y; cytr >= 0; --cytr)
		{
			struct map_tile *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cytr);
			if (map_tile_collision(tile->type))
			{
				cytr_tile = tile;
				break;
			}
		}
		
		int cyt = MAX(cytl, cytr);
		g_player.dist_top = g_player.pos_y - cyt - 1.0f;
		g_player.near_top = cytl > cytr ? cytl_tile : cytr_tile;
	}
	
	// find nearest bottom edge.
	{
		int cybl;
		struct map_tile *cybl_tile = NULL;
		for (cybl = g_player.pos_y + CONF_PLAYER_SIZE; cybl < g_map.size_y; ++cybl)
		{
			struct map_tile *tile = map_get(g_player.pos_x, cybl);
			if (map_tile_collision(tile->type))
			{
				cybl_tile = tile;
				break;
			}
		}
		
		int cybr;
		struct map_tile *cybr_tile = NULL;
		for (cybr = g_player.pos_y + CONF_PLAYER_SIZE; cybr < g_map.size_y; ++cybr)
		{
			struct map_tile *tile = map_get(g_player.pos_x + CONF_PLAYER_SIZE, cybr);
			if (map_tile_collision(tile->type))
			{
				cybr_tile = tile;
				break;
			}
		}
		
		int cyb = MIN(cybl, cybr);
		g_player.dist_bottom = cyb - g_player.pos_y - CONF_PLAYER_SIZE;
		g_player.near_bottom = cybl < cybr ? cybl_tile : cybr_tile;
	}
}

static void
test_and_apply_collisions(void)
{
	if (g_player.short_circuit)
		return;
	
	compute_collision_distances();
	if (-g_player.vel_y >= g_player.dist_top
	    && g_player.dist_top < g_player.dist_bottom)
	{
		collide(g_player.near_top);
		collide_top();
	}
	
	if (g_player.short_circuit)
		return;
	
	compute_collision_distances();
	if (-g_player.vel_x >= g_player.dist_left
	    && g_player.dist_left < g_player.dist_right)
	{
		collide(g_player.near_left);
		collide_left();
	}
	
	if (g_player.short_circuit)
		return;
	
	compute_collision_distances();
	if (g_player.vel_x >= g_player.dist_right
	    && g_player.dist_right < g_player.dist_left)
	{
		collide(g_player.near_right);
		collide_right();
	}
	
	if (g_player.short_circuit)
		return;
	
	compute_collision_distances();
	if (g_player.vel_y >= g_player.dist_bottom
	    && g_player.dist_bottom < g_player.dist_top)
	{
		collide(g_player.near_bottom);
		collide_bottom();
	}
}
