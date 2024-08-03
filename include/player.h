#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include <SDL.h>
#include <unistd.h>

typedef struct player
{
	float pos_x, pos_y;
	float vel_x, vel_y;
	float dist_left, dist_right, dist_top, dist_bottom;
	unsigned trace_spawn_ticks;
} player_t;

extern player_t g_player;

void player_update(void);
void player_draw(SDL_Renderer *rend);
bool player_grounded(void);
bool player_walled_left(void);
bool player_walled_right(void);

#endif
