#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef struct game
{
	unsigned off_switches;
	uint64_t il_time_ms;
} game_t;

extern game_t g_game;

void game_loop(void);
void game_disable_switches(void);

#endif
