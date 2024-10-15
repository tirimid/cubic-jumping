#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include <SDL2/SDL.h>

typedef enum mouse_button
{
	MB_LEFT = 0,
	MB_RIGHT,
	MB_MIDDLE,
	
	MB_END__,
} mouse_button;

// keyboard input functionality.
void keybd_set_key_state(SDL_Event const *e, bool pressed);
void keybd_post_update(void);
bool key_down(SDL_Keycode k);
bool key_pressed(SDL_Keycode k);
bool key_shift_held(void);

// mouse input functionality.
void mouse_press_button(SDL_Event const *e);
void mouse_release_button(SDL_Event const *e);
void mouse_post_update(void);
bool mouse_down(mouse_button b);
bool mouse_pressed(mouse_button b);
bool mouse_released(mouse_button b);
void mouse_pos(int *out_x, int *out_y);

#endif
