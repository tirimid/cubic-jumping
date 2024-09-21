#ifndef GAME_H
#define GAME_H

#include "map_list.h"

typedef struct game
{
	unsigned off_switches;
} game_t;

extern game_t g_game;

void game_init(map_list_item_t first_map);
void game_main_loop(void);
void game_disable_switches(void);

#endif
