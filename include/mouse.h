#ifndef MOUSE_H
#define MOUSE_H

#include <SDL.h>

typedef enum mouse_button
{
	MB_LEFT = 0,
	MB_RIGHT,
	MB_MIDDLE,
	
	MB_END__,
} mouse_button_t;

void mouse_press_button(SDL_Event const *e);
void mouse_release_button(SDL_Event const *e);
void mouse_post_update(void);
bool mouse_down(mouse_button_t b);
bool mouse_pressed(mouse_button_t b);
bool mouse_released(mouse_button_t b);
void mouse_pos(int *out_x, int *out_y);

#endif
