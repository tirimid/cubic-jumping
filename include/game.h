#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>

struct game
{
	uint64_t il_time_ms, total_time_ms;
	unsigned off_switches;
	unsigned il_deaths, total_deaths;
	bool running;
};

extern struct game g_game;

void game_loop(void);
void game_disable_switches(void);
void game_enable_switch(void);

#endif
