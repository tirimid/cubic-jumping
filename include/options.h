#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>

#include "util.h"

struct Options
{
	// keybind options.
	SDL_Keycode KLeft, KRight, KJump, KDashDown, KPowerjump;
	SDL_Keycode KMenu;
	SDL_Keycode KEditorLeft, KEditorRight, KEditorUp, KEditorDown;
	
	// sound options.
	f32 SfxVolume;
};

extern struct Options g_Options;

void Options_ReturnToDefault(char const *Path);
i32 Options_ReadFromFile(char const *Path);
i32 Options_WriteToFile(char const *Path);

#endif
