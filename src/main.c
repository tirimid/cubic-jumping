#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifdef BUILD_TARGET_WINDOWS
#include <Windows.h>
#endif

#include "conf.h"
#include "menus.h"
#include "options.h"
#include "sequences.h"
#include "sound.h"
#include "util.h"
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
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		{
			fprintf(stderr, "main: failed to init SDL2!\n");
			return 1;
		}
		atexit(SDL_Quit);
		
		if (Mix_Init(0))
		{
			log_err("main: failed to init SDL2 mixer!");
			return 1;
		}
		atexit(Mix_Quit);
		
		srand(time(NULL));
		
		SDL_StartTextInput();
	}
	
	// initialize game systems.
	{
		if (options_read_from_file(CONF_OPTIONS_FILE))
			options_return_to_default(CONF_OPTIONS_FILE);
		
		if (wnd_init())
			return 1;
		
		if (sound_init())
			return 1;
		sound_set_sfx_volume(g_options.sfx_volume);
	}
	
	// TODO: uncomment when intro sequence is done.
	//intro_sequence();
	
	main_menu_loop();
	
	return 0;
}
