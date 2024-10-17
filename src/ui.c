#include "ui.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "input.h"
#include "text.h"
#include "wnd.h"

ui_button
ui_button_create(int x, int y, char const *text, void (*callback)(void))
{
	return (ui_button)
	{
		.x = x,
		.y = y,
		.w = strlen(text) * TEXT_EFF_WIDTH,
		.h = TEXT_EFF_HEIGHT,
		.text = text,
		.callback = callback,
	};
}

void
ui_button_update(ui_button *btn)
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
ui_button_draw(ui_button const *btn)
{
	// set frame color based on button status.
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
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw button text.
	{
		text_draw_str(btn->text,
		              btn->x + CONF_BUTTON_PADDING,
		              btn->y + CONF_BUTTON_PADDING);
	}
}

ui_text_field
ui_text_field_create(int x, int y, size_t ndraw, char *out, size_t nmax)
{
	return (ui_text_field)
	{
		.x = x,
		.y = y,
		.w = TEXT_EFF_WIDTH * ndraw,
		.h = TEXT_EFF_HEIGHT,
		.out = out,
		.nmax = nmax,
		.ndraw = ndraw,
		.len = strlen(out),
	};
}

void
ui_text_field_update(ui_text_field *tf)
{
	int mouse_x, mouse_y;
	mouse_pos(&mouse_x, &mouse_y);
	
	// handle text field selection.
	{
		if (mouse_x >= tf->x
		    && mouse_x < tf->x + tf->w + 2 * CONF_TEXT_FIELD_PADDING
		    && mouse_y >= tf->y
		    && mouse_y < tf->y + tf->h + 2 * CONF_TEXT_FIELD_PADDING)
		{
			tf->hovered = true;
			if (mouse_pressed(MB_LEFT))
				tf->selected = true;
		}
		else
		{
			tf->hovered = false;
			if (mouse_pressed(MB_LEFT))
				tf->selected = false;
		}
	}
	
	if (!tf->selected)
		return;
	
	// handle text field keyboard navigation.
	{
		if (key_pressed(SDLK_LEFT))
		{
			tf->csr -= tf->csr > 0;
			tf->first_draw -= tf->csr < tf->first_draw;
		}
		
		if (key_pressed(SDLK_RIGHT))
		{
			tf->csr += tf->csr < tf->len;
			tf->first_draw += tf->csr - tf->first_draw >= tf->ndraw;
		}
		
		if (key_pressed(SDLK_UP))
		{
			tf->csr = 0;
			tf->first_draw = 0;
		}
		
		if (key_pressed(SDLK_DOWN))
		{
			tf->csr = tf->len;
			tf->first_draw = 0;
			while (tf->csr - tf->first_draw > tf->ndraw)
				++tf->first_draw;
		}
	}
	
	// handle text field keyboard input.
	{
		// support ASCII input.
		for (unsigned char i = 0; i < 128; ++i)
		{
			if (tf->len >= tf->nmax)
				break;
			
			if (!isprint(i))
				continue;
			
			if (key_text_input_received(i))
			{
				memmove(&tf->out[tf->csr + 1],
				        &tf->out[tf->csr],
				        tf->len - tf->csr);
				
				++tf->csr;
				tf->first_draw += tf->csr - tf->first_draw >= tf->ndraw;
				tf->out[tf->csr - 1] = i;
				
				++tf->len;
				tf->out[tf->len] = 0;
			}
		}
		
		if (key_pressed(SDLK_BACKSPACE) && tf->csr > 0)
		{
			memmove(&tf->out[tf->csr - 1],
			        &tf->out[tf->csr],
			        tf->len - tf->csr);
			
			--tf->csr;
			tf->first_draw -= tf->csr < tf->first_draw;
			
			--tf->len;
			tf->out[tf->len] = 0;
		}
	}
}

void
ui_text_field_draw(ui_text_field const *tf)
{
	// set frame color based on text field status.
	{
		static uint8_t ctf[] = CONF_COLOR_TEXT_FIELD;
		static uint8_t ctfs[] = CONF_COLOR_TEXT_FIELD_SELECTED;
		static uint8_t ctfh[] = CONF_COLOR_TEXT_FIELD_HOVERED;
		
		if (tf->selected)
			SDL_SetRenderDrawColor(g_rend, ctfs[0], ctfs[1], ctfs[2], 255);
		else if (tf->hovered)
			SDL_SetRenderDrawColor(g_rend, ctfh[0], ctfh[1], ctfh[2], 255);
		else
			SDL_SetRenderDrawColor(g_rend, ctf[0], ctf[1], ctf[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = tf->x,
			.y = tf->y,
			.w = tf->w + 2 * CONF_TEXT_FIELD_PADDING,
			.h = tf->h + 2 * CONF_TEXT_FIELD_PADDING,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw text field text.
	{
		int x = tf->x + CONF_TEXT_FIELD_PADDING;
		for (size_t i = tf->first_draw; i < tf->len; ++i)
		{
			if (x > tf->x + CONF_TEXT_FIELD_PADDING + tf->w)
				break;
			text_draw_ch(tf->out[i], x, tf->y + CONF_TEXT_FIELD_PADDING);
			x += TEXT_EFF_WIDTH;
		}
	}
	
	// draw cursor.
	{
		static uint8_t ctfc[] = CONF_COLOR_TEXT_FIELD_CURSOR;
		
		SDL_Rect r =
		{
			.x = tf->x + CONF_TEXT_FIELD_PADDING + (tf->csr - tf->first_draw) * TEXT_EFF_WIDTH - 2,
			.y = tf->y + CONF_TEXT_FIELD_PADDING,
			.w = CONF_TEXT_FIELD_CURSOR_WIDTH,
			.h = TEXT_EFF_HEIGHT,
		};
		
		SDL_SetRenderDrawColor(g_rend, ctfc[0], ctfc[1], ctfc[2], 255);
		SDL_RenderFillRect(g_rend, &r);
	}
}

ui_slider
ui_slider_create(int x,
                 int y,
                 int w,
                 int h,
                 float initial,
                 void (*callback)(float))
{
	return (ui_slider)
	{
		.x = x,
		.y = y,
		.w = w,
		.h = h,
		.val = initial,
		.callback = callback,
	};
}

void
ui_slider_update(ui_slider *s)
{
	int mouse_x, mouse_y;
	mouse_pos(&mouse_x, &mouse_y);
	
	if (mouse_x >= s->x
	    && mouse_x < s->x + s->w
	    && mouse_y >= s->y
	    && mouse_y < s->y + s->h)
	{
		s->hovered = true;
		if (mouse_down(MB_LEFT))
		{
			s->val = (float)(mouse_x - s->x) / s->w;
			if (s->callback)
				s->callback(s->val);
			s->pressed = true;
		}
		else if (mouse_released(MB_LEFT))
			s->pressed = false;
	}
	else
	{
		s->hovered = false;
		s->pressed = false;
	}
}

void
ui_slider_draw(ui_slider const *s)
{
	// set frame color based on slider state.
	{
		static uint8_t cs[] = CONF_COLOR_SLIDER;
		static uint8_t csh[] = CONF_COLOR_SLIDER_HOVERED;
		static uint8_t csp[] = CONF_COLOR_SLIDER_PRESSED;
		
		if (s->pressed)
			SDL_SetRenderDrawColor(g_rend, csp[0], csp[1], csp[2], 255);
		else if (s->hovered)
			SDL_SetRenderDrawColor(g_rend, csh[0], csh[1], csh[2], 255);
		else
			SDL_SetRenderDrawColor(g_rend, cs[0], cs[1], cs[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = s->x,
			.y = s->y,
			.w = s->w,
			.h = s->h,
		};
		SDL_RenderFillRect(g_rend, &r);
	}
	
	// draw slider cursor.
	{
		static uint8_t csc[] = CONF_COLOR_SLIDER_CURSOR;
		
		SDL_Rect r =
		{
			.x = s->x + s->val * s->w - CONF_SLIDER_CURSOR_WIDTH / 2.0f,
			.y = s->y,
			.w = CONF_SLIDER_CURSOR_WIDTH,
			.h = s->h,
		};
		
		SDL_SetRenderDrawColor(g_rend, csc[0], csc[1], csc[2], 255);
		SDL_RenderFillRect(g_rend, &r);
	}
}
