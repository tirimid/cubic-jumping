#include "ui.h"

#include <stdint.h>
#include <string.h>

#include <SDL.h>

#include "conf.h"
#include "input.h"
#include "text.h"
#include "wnd.h"

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
ui_button_update(ui_button_t *btn)
{
	int mouse_x, mouse_y;
	mouse_pos(&mouse_x, &mouse_y);
	
	if (mouse_x >= btn->x
	    && mouse_x < btn->x + btn->w + 2 * CONF_BUTTON_PADDING
	    && mouse_y >= btn->y
	    && mouse_y < btn->y + btn->h + 2 * CONF_BUTTON_PADDING)
	{
		btn->hovered = true;
		if (mouse_pressed(MB_LEFT))
			btn->pressed = true;
		else if (mouse_released(MB_LEFT))
		{
			if (btn->pressed && btn->callback)
				btn->callback();
			btn->pressed = false;
		}
	}
	else
	{
		btn->hovered = false;
		btn->pressed = false;
	}
}

void
ui_button_draw(ui_button_t const *btn)
{
	// set frame color based on button status.
	do
	{
		static uint8_t cb[] = CONF_COLOR_BUTTON;
		static uint8_t cbh[] = CONF_COLOR_BUTTON_HOVERED;
		static uint8_t cbp[] = CONF_COLOR_BUTTON_PRESSED;
		
		if (btn->pressed)
			SDL_SetRenderDrawColor(g_rend, cbp[0], cbp[1], cbp[2], 255);
		else if (btn->hovered)
			SDL_SetRenderDrawColor(g_rend, cbh[0], cbh[1], cbh[2], 255);
		else
			SDL_SetRenderDrawColor(g_rend, cb[0], cb[1], cb[2], 255);
	} while (0);
	
	// draw frame.
	do
	{
		SDL_Rect r =
		{
			.x = btn->x,
			.y = btn->y,
			.w = btn->w + 2 * CONF_BUTTON_PADDING,
			.h = btn->h + 2 * CONF_BUTTON_PADDING,
		};
		SDL_RenderFillRect(g_rend, &r);
	} while (0);
	
	// draw label text.
	do
	{
		text_draw_str(btn->text,
		              btn->x + CONF_BUTTON_PADDING,
		              btn->y + CONF_BUTTON_PADDING);
	} while (0);
}
