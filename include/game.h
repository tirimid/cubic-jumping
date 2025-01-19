#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>

struct Game
{
	uint64_t IlTimeMs, TotalTimeMs;
	unsigned OffSwitches;
	unsigned IlDeaths, TotalDeaths;
	bool Running;
};

extern struct Game g_Game;

void Game_Loop(void);
void Game_DisableSwitches(void);
void Game_EnableSwitch(void);

#endif
