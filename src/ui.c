#include "ui.h"

#include <ctype.h>
#include <string.h>

#include <SDL.h>

#include "conf.h"
#include "input.h"
#include "text.h"
#include "wnd.h"

struct UiButton
UiButton_Create(i32 x, i32 y, char const *Text, void (*Callback)(void))
{
	return (struct UiButton)
	{
		.x = x,
		.y = y,
		.w = strlen(Text) * TEXT_EFF_WIDTH,
		.h = TEXT_EFF_HEIGHT,
		.Text = Text,
		.Callback = Callback
	};
}

void
UiButton_Update(struct UiButton *Btn)
{
	i32 MouseX, MouseY;
	Mouse_Pos(&MouseX, &MouseY);
	
	if (MouseX >= Btn->x
		&& MouseX < Btn->x + Btn->w + 2 * CONF_BUTTON_PADDING
		&& MouseY >= Btn->y
		&& MouseY < Btn->y + Btn->h + 2 * CONF_BUTTON_PADDING)
	{
		Btn->Hovered = true;
		if (Mouse_Pressed(MB_LEFT))
			Btn->Pressed = true;
		else if (Mouse_Released(MB_LEFT))
		{
			if (Btn->Pressed && Btn->Callback)
				Btn->Callback();
			Btn->Pressed = false;
		}
	}
	else
	{
		Btn->Hovered = false;
		Btn->Pressed = false;
	}
}

void
UiButton_Draw(struct UiButton const *Btn)
{
	// set frame color based on button status.
	{
		static u8 Cb[] = CONF_COLOR_BUTTON;
		static u8 Cbh[] = CONF_COLOR_BUTTON_HOVERED;
		static u8 Cbp[] = CONF_COLOR_BUTTON_PRESSED;
		
		if (Btn->Pressed)
			SDL_SetRenderDrawColor(g_Rend, Cbp[0], Cbp[1], Cbp[2], 255);
		else if (Btn->Hovered)
			SDL_SetRenderDrawColor(g_Rend, Cbh[0], Cbh[1], Cbh[2], 255);
		else
			SDL_SetRenderDrawColor(g_Rend, Cb[0], Cb[1], Cb[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = Btn->x,
			.y = Btn->y,
			.w = Btn->w + 2 * CONF_BUTTON_PADDING,
			.h = Btn->h + 2 * CONF_BUTTON_PADDING
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw button text.
	{
		Text_DrawStr(
			Btn->Text,
			Btn->x + CONF_BUTTON_PADDING,
			Btn->y + CONF_BUTTON_PADDING
		);
	}
}

struct UiTextField
UiTextField_Create(i32 x, i32 y, u32 Ndraw, char *Out, u32 Nmax)
{
	return (struct UiTextField)
	{
		.x = x,
		.y = y,
		.w = TEXT_EFF_WIDTH * Ndraw,
		.h = TEXT_EFF_HEIGHT,
		.Out = Out,
		.Nmax = Nmax,
		.Ndraw = Ndraw,
		.Len = strlen(Out)
	};
}

void
UiTextField_Update(struct UiTextField *Tf)
{
	i32 MouseX, MouseY;
	Mouse_Pos(&MouseX, &MouseY);
	
	// handle text field selection.
	{
		if (MouseX >= Tf->x
			&& MouseX < Tf->x + Tf->w + 2 * CONF_TEXT_FIELD_PADDING
			&& MouseY >= Tf->y
			&& MouseY < Tf->y + Tf->h + 2 * CONF_TEXT_FIELD_PADDING)
		{
			Tf->Hovered = true;
			if (Mouse_Pressed(MB_LEFT))
				Tf->Selected = true;
		}
		else
		{
			Tf->Hovered = false;
			if (Mouse_Pressed(MB_LEFT))
				Tf->Selected = false;
		}
	}
	
	if (!Tf->Selected)
		return;
	
	// handle text field keyboard navigation.
	{
		if (Keybd_Pressed(SDLK_LEFT))
		{
			Tf->Csr -= Tf->Csr > 0;
			Tf->FirstDraw -= Tf->Csr < Tf->FirstDraw;
		}
		
		if (Keybd_Pressed(SDLK_RIGHT))
		{
			Tf->Csr += Tf->Csr < Tf->Len;
			Tf->FirstDraw += Tf->Csr - Tf->FirstDraw >= Tf->Ndraw;
		}
		
		if (Keybd_Pressed(SDLK_UP))
		{
			Tf->Csr = 0;
			Tf->FirstDraw = 0;
		}
		
		if (Keybd_Pressed(SDLK_DOWN))
		{
			Tf->Csr = Tf->Len;
			Tf->FirstDraw = 0;
			while (Tf->Csr - Tf->FirstDraw > Tf->Ndraw)
				++Tf->FirstDraw;
		}
	}
	
	// handle text field keyboard input.
	{
		// support ASCII input.
		for (u8 i = 0; i < 128; ++i)
		{
			if (Tf->Len >= Tf->Nmax)
				break;
			
			if (!isprint(i))
				continue;
			
			if (Keybd_TextInputReceived(i))
			{
				memmove(
					&Tf->Out[Tf->Csr + 1],
					&Tf->Out[Tf->Csr],
					Tf->Len - Tf->Csr
				);
				
				++Tf->Csr;
				Tf->FirstDraw += Tf->Csr - Tf->FirstDraw >= Tf->Ndraw;
				Tf->Out[Tf->Csr - 1] = i;
				
				++Tf->Len;
				Tf->Out[Tf->Len] = 0;
			}
		}
		
		if (Keybd_Pressed(SDLK_BACKSPACE) && Tf->Csr > 0)
		{
			memmove(
				&Tf->Out[Tf->Csr - 1],
				&Tf->Out[Tf->Csr],
				Tf->Len - Tf->Csr
			);
			
			--Tf->Csr;
			Tf->FirstDraw -= Tf->Csr < Tf->FirstDraw;
			
			--Tf->Len;
			Tf->Out[Tf->Len] = 0;
		}
	}
}

void
UiTextField_Draw(struct UiTextField const *Tf)
{
	// set frame color based on text field status.
	{
		static u8 Ctf[] = CONF_COLOR_TEXT_FIELD;
		static u8 Ctfs[] = CONF_COLOR_TEXT_FIELD_SELECTED;
		static u8 Ctfh[] = CONF_COLOR_TEXT_FIELD_HOVERED;
		
		if (Tf->Selected)
			SDL_SetRenderDrawColor(g_Rend, Ctfs[0], Ctfs[1], Ctfs[2], 255);
		else if (Tf->Hovered)
			SDL_SetRenderDrawColor(g_Rend, Ctfh[0], Ctfh[1], Ctfh[2], 255);
		else
			SDL_SetRenderDrawColor(g_Rend, Ctf[0], Ctf[1], Ctf[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = Tf->x,
			.y = Tf->y,
			.w = Tf->w + 2 * CONF_TEXT_FIELD_PADDING,
			.h = Tf->h + 2 * CONF_TEXT_FIELD_PADDING
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw text field text.
	{
		i32 x = Tf->x + CONF_TEXT_FIELD_PADDING;
		for (u32 i = Tf->FirstDraw; i < Tf->Len; ++i)
		{
			if (x > Tf->x + CONF_TEXT_FIELD_PADDING + Tf->w)
				break;
			Text_DrawCh(Tf->Out[i], x, Tf->y + CONF_TEXT_FIELD_PADDING);
			x += TEXT_EFF_WIDTH;
		}
	}
	
	// draw cursor.
	{
		static u8 Ctfc[] = CONF_COLOR_TEXT_FIELD_CURSOR;
		
		SDL_Rect r =
		{
			.x = Tf->x + CONF_TEXT_FIELD_PADDING + (Tf->Csr - Tf->FirstDraw) * TEXT_EFF_WIDTH - 2,
			.y = Tf->y + CONF_TEXT_FIELD_PADDING,
			.w = CONF_TEXT_FIELD_CURSOR_WIDTH,
			.h = TEXT_EFF_HEIGHT
		};
		
		SDL_SetRenderDrawColor(g_Rend, Ctfc[0], Ctfc[1], Ctfc[2], 255);
		SDL_RenderFillRect(g_Rend, &r);
	}
}

struct UiSlider
UiSlider_Create(i32 x, i32 y, i32 w, i32 h, f32 Initial, void (*Callback)(f32))
{
	return (struct UiSlider)
	{
		.x = x,
		.y = y,
		.w = w,
		.h = h,
		.Val = Initial,
		.Callback = Callback
	};
}

void
UiSlider_Update(struct UiSlider *s)
{
	i32 MouseX, MouseY;
	Mouse_Pos(&MouseX, &MouseY);
	
	if (MouseX >= s->x
		&& MouseX < s->x + s->w
		&& MouseY >= s->y
		&& MouseY < s->y + s->h)
	{
		s->Hovered = true;
		if (Mouse_Down(MB_LEFT))
		{
			s->Val = (f32)(MouseX - s->x) / s->w;
			if (s->Callback)
				s->Callback(s->Val);
			s->Pressed = true;
		}
		else if (Mouse_Released(MB_LEFT))
			s->Pressed = false;
	}
	else
	{
		s->Hovered = false;
		s->Pressed = false;
	}
}

void
UiSlider_Draw(struct UiSlider const *s)
{
	// set frame color based on slider state.
	{
		static u8 Cs[] = CONF_COLOR_SLIDER;
		static u8 Csh[] = CONF_COLOR_SLIDER_HOVERED;
		static u8 Csp[] = CONF_COLOR_SLIDER_PRESSED;
		
		if (s->Pressed)
			SDL_SetRenderDrawColor(g_Rend, Csp[0], Csp[1], Csp[2], 255);
		else if (s->Hovered)
			SDL_SetRenderDrawColor(g_Rend, Csh[0], Csh[1], Csh[2], 255);
		else
			SDL_SetRenderDrawColor(g_Rend, Cs[0], Cs[1], Cs[2], 255);
	}
	
	// draw frame.
	{
		SDL_Rect r =
		{
			.x = s->x,
			.y = s->y,
			.w = s->w,
			.h = s->h
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw slider cursor.
	{
		static u8 Csc[] = CONF_COLOR_SLIDER_CURSOR;
		
		SDL_Rect r =
		{
			.x = s->x + s->Val * s->w - CONF_SLIDER_CURSOR_WIDTH / 2.0f,
			.y = s->y,
			.w = CONF_SLIDER_CURSOR_WIDTH,
			.h = s->h
		};
		
		SDL_SetRenderDrawColor(g_Rend, Csc[0], Csc[1], Csc[2], 255);
		SDL_RenderFillRect(g_Rend, &r);
	}
}
