#ifndef UI_H
#define UI_H

#include <stdbool.h>

typedef struct ui_button
{
	void (*callback)(void);
	char const *text;
	int x, y;
	int w, h;
	bool hovered, pressed;
} ui_button_t;

ui_button_t ui_button_create(int x, int y, char const *text, void (*callback)(void));
void ui_button_update(ui_button_t *btn);
void ui_button_draw(ui_button_t const *btn);

#endif
