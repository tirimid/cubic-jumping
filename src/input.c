#include "input.h"

#include <stdlib.h>
#include <string.h>

// just assume that 1024 is big enough to fit all needed inputs.
static u8 KDownStates[1024 / 8], KPressStates[1024 / 8];
static u8 KTextInputStates[128 / 8];
static u8 MDownStates[MB_END__], MPressStates[MB_END__], MReleaseStates[MB_END__];

void
Input_HandleEvents(void)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			exit(0);
		case SDL_KEYDOWN:
			if (!e.key.repeat)
				Keybd_SetKeyState(&e, true);
			break;
		case SDL_KEYUP:
			if (!e.key.repeat)
				Keybd_SetKeyState(&e, false);
			break;
		case SDL_MOUSEBUTTONUP:
			Mouse_ReleaseButton(&e);
			break;
		case SDL_MOUSEBUTTONDOWN:
			Mouse_PressButton(&e);
			break;
		case SDL_TEXTINPUT:
			Keybd_RegisterTextInput(&e);
			break;
		default:
			break;
		}
	}
}

void
Input_PostUpdate(void)
{
	Keybd_PostUpdate();
	Mouse_PostUpdate();
}

void
Keybd_SetKeyState(SDL_Event const *e, bool Pressed)
{
	SDL_Keycode k = e->key.keysym.sym;
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize Byte = k / 8, Bit = k % 8;
	
	KDownStates[Byte] &= ~(1 << Bit);
	KDownStates[Byte] |= Pressed << Bit;
	
	KPressStates[Byte] &= ~(1 << Bit);
	KPressStates[Byte] |= Pressed << Bit;
}

void
Keybd_RegisterTextInput(SDL_Event const *e)
{
	u8 Ch = e->text.text[0];
	
	// disregard non-ASCII input.
	if (Ch & 0x80)
		return;
	
	usize Byte = Ch / 8, Bit = Ch % 8;
	
	KTextInputStates[Byte] |= 1 << Bit;
}

void
Keybd_PostUpdate(void)
{
	memset(KPressStates, 0, sizeof(KPressStates));
	memset(KTextInputStates, 0, sizeof(KTextInputStates));
}

bool
Keybd_Down(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize Byte = k / 8, Bit = k % 8;
	
	return KDownStates[Byte] & 1 << Bit;
}

bool
Keybd_Pressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize Byte = k / 8, Bit = k % 8;
	
	return KPressStates[Byte] & 1 << Bit;
}

bool
Keybd_TextInputReceived(char Ch)
{
	usize Byte = Ch / 8, Bit = Ch % 8;
	return KTextInputStates[Byte] & 1 << Bit;
}

bool
Keybd_ShiftHeld(void)
{
	SDL_Keymod Km = SDL_GetModState();
	return Km & KMOD_LSHIFT || Km & KMOD_RSHIFT;
}

void
Mouse_PressButton(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		MDownStates[MB_LEFT] = MPressStates[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		MDownStates[MB_RIGHT] = MPressStates[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		MDownStates[MB_MIDDLE] = MPressStates[MB_MIDDLE] = true;
		break;
	}
}

void
Mouse_ReleaseButton(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		MDownStates[MB_LEFT] = false;
		MReleaseStates[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		MDownStates[MB_RIGHT] = false;
		MReleaseStates[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		MDownStates[MB_MIDDLE] = false;
		MReleaseStates[MB_RIGHT] = true;
		break;
	}
}

void
Mouse_PostUpdate(void)
{
	MPressStates[MB_LEFT] = MReleaseStates[MB_LEFT] = false;
	MPressStates[MB_RIGHT] = MReleaseStates[MB_RIGHT] = false;
	MPressStates[MB_MIDDLE] = MReleaseStates[MB_MIDDLE] = false;
}

bool
Mouse_Down(enum MouseButton b)
{
	return MDownStates[b];
}

bool
Mouse_Pressed(enum MouseButton b)
{
	return MPressStates[b];
}

bool
Mouse_Released(enum MouseButton b)
{
	return MReleaseStates[b];
}

void
Mouse_Pos(i32 *OutX, i32 *OutY)
{
	SDL_GetMouseState(OutX, OutY);
}
