#include "input.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// just assume that 1024 is a big enough array size.
static bool k_down_states[1024], k_press_states[1024];
static bool k_text_input_states[128];
static bool m_down_states[MB_END__], m_press_states[MB_END__], m_release_states[MB_END__];

void
input_handle_events(void)
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
				keybd_set_key_state(&e, true);
			break;
		case SDL_KEYUP:
			if (!e.key.repeat)
				keybd_set_key_state(&e, false);
			break;
		case SDL_MOUSEBUTTONUP:
			mouse_release_button(&e);
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouse_press_button(&e);
			break;
		case SDL_TEXTINPUT:
			keybd_register_text_input(&e);
			break;
		default:
			break;
		}
	}
}

void
input_post_update(void)
{
	keybd_post_update();
	mouse_post_update();
}

void
keybd_set_key_state(SDL_Event const *e, bool pressed)
{
	SDL_Keycode k = e->key.keysym.sym;
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	k_down_states[k] = k_press_states[k] = pressed;
}

void
keybd_register_text_input(SDL_Event const *e)
{
	unsigned char ch = e->text.text[0];
	
	// disregard non-ASCII input.
	if (ch & 0x80)
		return;
	
	k_text_input_states[ch] = true;
}

void
keybd_post_update(void)
{
	memset(k_press_states, 0, sizeof(k_press_states));
	memset(k_text_input_states, 0, sizeof(k_text_input_states));
}

bool
key_down(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	return k_down_states[k];
}

bool
key_pressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	return k_press_states[k];
}

bool
key_text_input_received(char ch)
{
	return k_text_input_states[ch];
}

bool
key_shift_held(void)
{
	SDL_Keymod km = SDL_GetModState();
	return km & KMOD_LSHIFT || km & KMOD_RSHIFT;
}

void
mouse_press_button(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		m_down_states[MB_LEFT] = m_press_states[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		m_down_states[MB_RIGHT] = m_press_states[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		m_down_states[MB_MIDDLE] = m_press_states[MB_MIDDLE] = true;
		break;
	}
}

void
mouse_release_button(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		m_down_states[MB_LEFT] = false;
		m_release_states[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		m_down_states[MB_RIGHT] = false;
		m_release_states[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		m_down_states[MB_MIDDLE] = false;
		m_release_states[MB_RIGHT] = true;
		break;
	}
}

void
mouse_post_update(void)
{
	m_press_states[MB_LEFT] = m_release_states[MB_LEFT] = false;
	m_press_states[MB_RIGHT] = m_release_states[MB_RIGHT] = false;
	m_press_states[MB_MIDDLE] = m_release_states[MB_MIDDLE] = false;
}

bool
mouse_down(mouse_button b)
{
	return m_down_states[b];
}

bool
mouse_pressed(mouse_button b)
{
	return m_press_states[b];
}

bool
mouse_released(mouse_button b)
{
	return m_release_states[b];
}

void
mouse_pos(int *out_x, int *out_y)
{
	SDL_GetMouseState(out_x, out_y);
}
