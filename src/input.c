#include "input.h"

#include <stdint.h>
#include <string.h>

// just assume that 1024 is a big enough array size.
static bool k_down_states[1024], k_press_states[1024];
static bool m_down_states[MB_END__], m_press_states[MB_END__], m_release_states[MB_END__];

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
keybd_post_update(void)
{
	memset(k_press_states, 0, sizeof(k_press_states));
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
