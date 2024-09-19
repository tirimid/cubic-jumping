#include "ui.h"

#include <stdint.h>
#include <string.h>

#include "conf.h"
#include "text.h"

#define MOUSE_LEFT 1

ui_button_t
ui_button_create(int x, int y, char const *text, void (*callback)(void))
{
	return (ui_button_t)
	{
		.x = x,
		.y = y,
		.w = strlen(text) * CONF_TEXT_SCALE * (TEXT_FONT_WIDTH + 0.5f),
		.h = CONF_TEXT_SCALE * TEXT_FONT_HEIGHT,
		.text = text,
		.callback = callback,
	};
}

void
ui_button_proc_event(ui_button_t *btn, SDL_Event const *e)
{
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	
	if (mouse_x >= btn->x
	    && mouse_x < btn->x + btn->w + 2 * CONF_BUTTON_PADDING
	    && mouse_y >= btn->y
	    && mouse_y < btn->y + btn->h + 2 * CONF_BUTTON_PADDING)
	{
		if (e->button.button == MOUSE_LEFT)
		{
			if (e->type == SDL_MOUSEBUTTONDOWN)
				btn->pressed = true;
			else if (e->type == SDL_MOUSEBUTTONUP)
			{
				if (btn->pressed && btn->callback)
					btn->callback();
				btn->pressed = false;
			}
		}
	}
}

void
ui_button_update(ui_button_t *btn)
{
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	
	if (mouse_x >= btn->x
	    && mouse_x < btn->x + btn->w + 2 * CONF_BUTTON_PADDING
	    && mouse_y >= btn->y
	    && mouse_y < btn->y + btn->h + 2 * CONF_BUTTON_PADDING)
	{
		btn->hovered = true;
	}
	else
	{
		btn->hovered = false;
		btn->pressed = false;
	}
}

void
ui_button_draw(SDL_Renderer *rend, ui_button_t const *btn)
{
	// set frame color based on button status.
	{
		static uint8_t cb[] = CONF_COLOR_BUTTON;
		static uint8_t cbh[] = CONF_COLOR_BUTTON_HOVERED;
		static uint8_t cbp[] = CONF_COLOR_BUTTON_PRESSED;
		
		if (btn->pressed)
			SDL_SetRenderDrawColor(rend, cbp[0], cbp[1], cbp[2], 255);
		else if (btn->hovered)
			SDL_SetRenderDrawColor(rend, cbh[0], cbh[1], cbh[2], 255);
		else
			SDL_SetRenderDrawColor(rend, cb[0], cb[1], cb[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = btn->x,
			.y = btn->y,
			.w = btn->w + 2 * CONF_BUTTON_PADDING,
			.h = btn->h + 2 * CONF_BUTTON_PADDING,
		};
		SDL_RenderFillRect(rend, &r);
	}
	
	// draw label text.
	{
		text_draw_str(rend,
		              btn->text,
		              btn->x + CONF_BUTTON_PADDING,
		              btn->y + CONF_BUTTON_PADDING);
	}
}
