#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

#include "menus.h"
#include "wnd.h"

int
main(int argc, char *argv[])
{
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
	
	if (wnd_init())
		return 1;
	
	main_menu_loop();
	
	return 0;
}
