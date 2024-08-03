#include <stdio.h>

#include <SDL.h>

#include "game.h"
#include "util.h"

int
main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (game_init())
		return 1;
	
	game_main_loop();
	
	return 0;
}
