#ifndef KEYBD_H
#define KEYBD_H

#include <stdbool.h>

#include <SDL.h>

typedef enum key
{
	K_LEFT = 0,
	K_RIGHT,
	K_JUMP,
	K_FALL,
	K_POWERJUMP,
	K_ACT,
	K_MENU,
	K_SELECT,
	
	K_END__,
} key_t;

void keybd_set_key_state(SDL_Event const *e, bool pressed);
void keybd_post_update(void);
bool key_down(key_t k);
bool key_pressed(key_t k);

#endif
