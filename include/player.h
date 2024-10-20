#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include <unistd.h>

#include "map.h"

enum player_state
{
	PS_PLAYING = 0,
	PS_DEAD,
};

enum player_cap_id
{
	PCI_JUMP = 0,
	PCI_WALLJUMP,
	PCI_WALLSLIDE,
	PCI_POWERJUMP,
	PCI_DASH_DOWN,
};

struct player_cap_mask
{
	bool no_jump, no_walljump, no_wallslide;
	bool no_powerjump, no_dash_down;
};

struct player
{
	// movement data.
	float pos_x, pos_y;
	float vel_x, vel_y;
	bool air_control;
	
	// collision handling data.
	float dist_left, dist_right, dist_top, dist_bottom;
	struct map_tile *near_left, *near_right, *near_top, *near_bottom;
	bool short_circuit;
	
	// visual data.
	unsigned trace_spawn_ticks;
	unsigned dead_ticks;
};

extern struct player g_player;
extern enum player_state g_player_state;
extern struct player_cap_mask g_player_cap_mask;

void player_update(void);
void player_draw(void);
bool player_grounded(void);
bool player_walled_left(void);
bool player_walled_right(void);
void player_die(void);
void player_set_cap_mask(enum player_cap_id id, bool state);

#endif
