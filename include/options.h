#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL.h>

#include "util.h"

struct Options
{
	// keybind options.
	SDL_Keycode KLeft, KRight, KJump, KDashDown, KPowerjump;
	SDL_Keycode KMenu;
	SDL_Keycode KEditorLeft, KEditorRight, KEditorUp, KEditorDown;
	SDL_Keycode KEditorQuickSelect;
	
	// sound options.
	f32 SfxVolume, MusicVolume;
};

extern struct Options g_Options;

void Options_ReturnToDefault(char const *Path);
i32 Options_ReadFromFile(char const *Path);
i32 Options_WriteToFile(char const *Path);

#endif
