#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ui_button
{
	void (*callback)(void);
	char const *text;
	int x, y;
	int w, h;
	bool hovered, pressed;
} ui_button;

typedef struct ui_text_box
{
	char *out;
	size_t csr, nmax;
	size_t first_draw, ndraw;
	int x, y;
	bool selected;
} ui_text_box;

ui_button ui_button_create(int x, int y, char const *text, void (*callback)(void));
void ui_button_update(ui_button *btn);
void ui_button_draw(ui_button const *btn);
ui_text_box ui_text_box_create(int x, int y, size_t ndraw, char *out, size_t nmax);
void ui_text_box_update(ui_text_box *tb);
void ui_text_box_draw(ui_text_box const *tb);

#endif
