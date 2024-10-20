#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stddef.h>

struct ui_button
{
	void (*callback)(void);
	char const *text;
	int x, y;
	int w, h;
	bool hovered, pressed;
};

struct ui_text_field
{
	char *out;
	size_t csr, len, nmax;
	size_t first_draw, ndraw;
	int x, y;
	int w, h;
	bool hovered, selected;
};

struct ui_slider
{
	void (*callback)(float);
	int x, y;
	int w, h;
	float val;
	bool hovered, pressed;
};

struct ui_button ui_button_create(int x, int y, char const *text, void (*callback)(void));
void ui_button_update(struct ui_button *btn);
void ui_button_draw(struct ui_button const *btn);
struct ui_text_field ui_text_field_create(int x, int y, size_t ndraw, char *out, size_t nmax);
void ui_text_field_update(struct ui_text_field *tb);
void ui_text_field_draw(struct ui_text_field const *tb);
struct ui_slider ui_slider_create(int x, int y, int w, int h, float initial, void (*callback)(float));
void ui_slider_update(struct ui_slider *s);
void ui_slider_draw(struct ui_slider const *s);

#endif
