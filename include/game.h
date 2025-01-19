#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "util.h"

struct Game
{
	u64 IlTimeMs, TotalTimeMs;
	u32 OffSwitches;
	u32 IlDeaths, TotalDeaths;
	bool Running;
};

extern struct Game g_Game;

void Game_Loop(void);
void Game_DisableSwitches(void);
void Game_EnableSwitch(void);

#endif
