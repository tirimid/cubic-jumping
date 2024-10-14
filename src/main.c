#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>

#ifdef BUILD_TARGET_WINDOWS
#include <Windows.h>
#endif

#include "menus.h"
#include "wnd.h"

#ifdef BUILD_TARGET_WINDOWS
#define ENTRY_FN \
	int APIENTRY \
	WinMain(HINSTANCE h_inst, \
	        HINSTANCE h_prev_inst, \
	        LPSTR lp_cmd_line, \
	        int n_cmd_show)
#else
#define ENTRY_FN \
	int \
	main(void)
#endif

ENTRY_FN
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
