#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include <SDL2/SDL.h>

enum mouse_button
{
	MB_LEFT = 0,
	MB_RIGHT,
	MB_MIDDLE,
	
	MB_END__,
};

void input_handle_events(void);
void input_post_update(void);

// keyboard input functionality.
void keybd_set_key_state(SDL_Event const *e, bool pressed);
void keybd_register_text_input(SDL_Event const *e);
void keybd_post_update(void);
bool key_down(SDL_Keycode k);
bool key_pressed(SDL_Keycode k);
bool key_text_input_received(char ch);
bool key_shift_held(void);

// mouse input functionality.
void mouse_press_button(SDL_Event const *e);
void mouse_release_button(SDL_Event const *e);
void mouse_post_update(void);
bool mouse_down(enum mouse_button b);
bool mouse_pressed(enum mouse_button b);
bool mouse_released(enum mouse_button b);
void mouse_pos(int *out_x, int *out_y);

#endif
