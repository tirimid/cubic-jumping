#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "editor.h"
#include "game.h"
#include "map.h"
#include "util.h"

typedef enum game_mode
{
	GM_GAME = 0,
	GM_EDITOR,
} game_mode_t;

static void usage(char const *p_name);

int
main(int argc, char *argv[])
{
	// check game launch mode and CLI usage.
	game_mode_t mode;
	{
		if (argc < 2)
			mode = GM_GAME;
		else if (!strcmp(argv[1], "play"))
		{
			if (argc != 2)
			{
				usage(argv[0]);
				return 1;
			}
			
			mode = GM_GAME;
		}
		else if (!strcmp(argv[1], "edit"))
		{
			if (argc != 3)
			{
				usage(argv[0]);
				return 1;
			}
			
			mode = GM_EDITOR;
		}
		else if (!strcmp(argv[1], "hfm"))
		{
			if (argc != 4)
			{
				usage(argv[0]);
				return 1;
			}
			
			if (map_create_file(argv[2], argv[3]))
				return 1;
			
			return 0;
		}
		else
		{
			usage(argv[0]);
			return 1;
		}
	}
	
	// initialize non-game systems.
	{
		if (SDL_Init(SDL_INIT_VIDEO))
		{
			fprintf(stderr, "main: failed to init SDL2!\n");
			return 1;
		}
		atexit(SDL_Quit);
		
		srand(time(NULL));
	}
	
	switch (mode)
	{
	case GM_GAME:
		if (game_init())
			return 1;
		game_main_loop();
		break;
	case GM_EDITOR:
		if (editor_init(argv[2]))
			return 1;
		editor_main_loop();
		break;
	}
	
	return 0;
}

static void
usage(char const *p_name)
{
	fprintf(stderr,
	        "main: invalid usage!\n"
	        "usage:\n"
	        "\t%s                    launch game\n"
	        "\t%s play               launch game\n"
	        "\t%s edit <file>        edit HFM map\n"
	        "\t%s hfm <file> <name>  create new HFM map\n",
	        p_name,
	        p_name,
	        p_name,
	        p_name);
}
