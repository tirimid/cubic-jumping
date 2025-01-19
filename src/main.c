#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#ifdef BUILD_TARGET_WINDOWS
#include <Windows.h>
#endif

#include "conf.h"
#include "menus.h"
#include "options.h"
#include "sequences.h"
#include "sound.h"
#include "textures.h"
#include "util.h"
#include "wnd.h"

#define INIT_IMG_FLAGS IMG_INIT_PNG

#ifdef BUILD_TARGET_WINDOWS
#define ENTRY_FN \
	i32 APIENTRY \
	WinMain( \
		HINSTANCE HInst, \
		HINSTANCE HPrevInst, \
		LPSTR LpCmdLine, \
		i32 NCmdShow \
	)
#else
#define ENTRY_FN \
	i32 \
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
			LogErr("main: failed to init SDL2 mixer!");
			return 1;
		}
		atexit(Mix_Quit);
		
		if ((IMG_Init(INIT_IMG_FLAGS) & INIT_IMG_FLAGS) != INIT_IMG_FLAGS)
		{
			LogErr("main: failed to init SDL2 image!");
			return 1;
		}
		atexit(IMG_Quit);
		
		srand(time(NULL));
		
		SDL_StartTextInput();
	}
	
	// initialize game systems.
	{
		if (Options_ReadFromFile(CONF_OPTIONS_FILE))
			Options_ReturnToDefault(CONF_OPTIONS_FILE);
		
		if (Wnd_Init())
			return 1;
		
		if (Sound_Init())
			return 1;
		Sound_SetSfxVolume(g_Options.SfxVolume);
		
		if (Textures_Init())
			return 1;
	}
	
	IntroSequence();
	MainMenuLoop();
	
	return 0;
}
