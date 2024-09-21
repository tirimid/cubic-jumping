#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include <unistd.h>

#include "map.h"

typedef enum player_state
{
	PS_PLAYING = 0,
	PS_DEAD,
} player_state_t;

typedef struct player
{
	// movement data.
	float pos_x, pos_y;
	float vel_x, vel_y;
	
	// collision handling data.
	float dist_left, dist_right, dist_top, dist_bottom;
	map_tile_t const *near_left, *near_right, *near_top, *near_bottom;
	
	// visual data.
	unsigned trace_spawn_ticks;
	unsigned dead_ticks;
} player_t;

extern player_t g_player;
extern player_state_t g_player_state;

void player_update(void);
void player_draw(void);
bool player_grounded(void);
bool player_walled_left(void);
bool player_walled_right(void);
void player_die(void);

#endif
