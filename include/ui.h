#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include <SDL.h>

typedef struct ui_button
{
	void (*callback)(void);
	char const *text;
	int x, y;
	int w, h;
	bool hovered, pressed;
} ui_button_t;

ui_button_t ui_button_create(int x, int y, char const *text, void (*callback)(void));
void ui_button_proc_event(ui_button_t *btn, SDL_Event const *e);
void ui_button_update(ui_button_t *btn);
void ui_button_draw(SDL_Renderer *rend, ui_button_t const *btn);

#endif
