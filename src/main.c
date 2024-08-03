#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#include "editor.h"
#include "game.h"
#include "util.h"

typedef enum game_mode
{
	GM_GAME = 0,
	GM_EDITOR,
} game_mode_t;

int
main(int argc, char *argv[])
{
	srand(time(NULL));
	
	game_mode_t mode;
	if (argc == 1)
		mode = GM_GAME;
	else if (argc == 3)
		mode = GM_EDITOR;
	else
	{
		fprintf(stderr, "main: invalid usage for game and editor!\n");
		return 1;
	}
	
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "main: failed to init SDL2!\n");
		return 1;
	}
	atexit(SDL_Quit);
	
	switch (mode)
	{
	case GM_GAME:
		if (game_init())
			return 1;
		game_main_loop();
		break;
	case GM_EDITOR:
		if (editor_init(argv[1], argv[2]))
			return 1;
		editor_main_loop();
		break;
	}
	
	return 0;
}
