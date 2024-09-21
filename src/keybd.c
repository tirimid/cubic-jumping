#include "keybd.h"

#include "conf.h"

static bool down_states[K_END__], press_states[K_END__];

void
keybd_set_key_state(SDL_Event const *e, bool pressed)
{
	switch (e->key.keysym.sym)
	{
	case CONF_KEY_LEFT:
		down_states[K_LEFT] = press_states[K_LEFT] = pressed;
		break;
	case CONF_KEY_RIGHT:
		down_states[K_RIGHT] = press_states[K_RIGHT] = pressed;
		break;
	case CONF_KEY_JUMP:
		down_states[K_JUMP] = press_states[K_JUMP] = pressed;
		break;
	case CONF_KEY_FALL:
		down_states[K_FALL] = press_states[K_FALL] = pressed;
		break;
	case CONF_KEY_POWERJUMP:
		down_states[K_POWERJUMP] = press_states[K_POWERJUMP] = pressed;
		break;
	case CONF_KEY_MENU:
		down_states[K_MENU] = press_states[K_MENU] = pressed;
		break;
	default:
		break;
	}
}

void
keybd_post_update(void)
{
	press_states[K_LEFT] = false;
	press_states[K_RIGHT] = false;
	press_states[K_JUMP] = false;
	press_states[K_FALL] = false;
	press_states[K_POWERJUMP] = false;
	press_states[K_MENU] = false;
}

bool
key_down(key_t k)
{
	return down_states[k];
}

bool
key_pressed(key_t k)
{
	return press_states[k];
}
