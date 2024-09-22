#include "input.h"

#include "conf.h"

static bool k_down_states[K_END__], k_press_states[K_END__];
static bool m_down_states[MB_END__], m_press_states[MB_END__], m_release_states[MB_END__];

void
keybd_set_key_state(SDL_Event const *e, bool pressed)
{
	switch (e->key.keysym.sym)
	{
	case CONF_KEY_LEFT:
		k_down_states[K_LEFT] = k_press_states[K_LEFT] = pressed;
		break;
	case CONF_KEY_RIGHT:
		k_down_states[K_RIGHT] = k_press_states[K_RIGHT] = pressed;
		break;
	case CONF_KEY_JUMP:
		k_down_states[K_JUMP] = k_press_states[K_JUMP] = pressed;
		break;
	case CONF_KEY_FALL:
		k_down_states[K_FALL] = k_press_states[K_FALL] = pressed;
		break;
	case CONF_KEY_POWERJUMP:
		k_down_states[K_POWERJUMP] = k_press_states[K_POWERJUMP] = pressed;
		break;
	case CONF_KEY_MENU:
		k_down_states[K_MENU] = k_press_states[K_MENU] = pressed;
		break;
	default:
		break;
	}
}

void
keybd_post_update(void)
{
	k_press_states[K_LEFT] = false;
	k_press_states[K_RIGHT] = false;
	k_press_states[K_JUMP] = false;
	k_press_states[K_FALL] = false;
	k_press_states[K_POWERJUMP] = false;
	k_press_states[K_MENU] = false;
}

bool
key_down(key_t k)
{
	return k_down_states[k];
}

bool
key_pressed(key_t k)
{
	return k_press_states[k];
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
mouse_down(mouse_button_t b)
{
	return m_down_states[b];
}

bool
mouse_pressed(mouse_button_t b)
{
	return m_press_states[b];
}

bool
mouse_released(mouse_button_t b)
{
	return m_release_states[b];
}

void
mouse_pos(int *out_x, int *out_y)
{
	SDL_GetMouseState(out_x, out_y);
}
