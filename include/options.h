#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>

struct options
{
	// keybind options.
	SDL_Keycode k_left, k_right, k_jump, k_dash_down, k_powerjump;
	SDL_Keycode k_menu;
	SDL_Keycode k_editor_left, k_editor_right, k_editor_up, k_editor_down;
	
	// sound options.
	float sfx_volume;
};

extern struct options g_options;

void options_return_to_default(char const *path);
int options_read_from_file(char const *path);
int options_write_to_file(char const *path);

#endif
