#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>

struct Options
{
	// keybind options.
	SDL_Keycode KLeft, KRight, KJump, KDashDown, KPowerjump;
	SDL_Keycode KMenu;
	SDL_Keycode KEditorLeft, KEditorRight, KEditorUp, KEditorDown;
	
	// sound options.
	float SfxVolume;
};

extern struct Options g_Options;

void Options_ReturnToDefault(char const *Path);
int Options_ReadFromFile(char const *Path);
int Options_WriteToFile(char const *Path);

#endif
