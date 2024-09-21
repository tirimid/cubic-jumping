#include "mouse.h"

static bool down_states[MB_END__], press_states[MB_END__], release_states[MB_END__];

void
mouse_press_button(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		down_states[MB_LEFT] = press_states[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		down_states[MB_RIGHT] = press_states[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		down_states[MB_MIDDLE] = press_states[MB_MIDDLE] = true;
		break;
	}
}

void
mouse_release_button(SDL_Event const *e)
{
	switch (e->button.button)
	{
	case SDL_BUTTON_LEFT:
		down_states[MB_LEFT] = false;
		release_states[MB_LEFT] = true;
		break;
	case SDL_BUTTON_RIGHT:
		down_states[MB_RIGHT] = false;
		release_states[MB_RIGHT] = true;
		break;
	case SDL_BUTTON_MIDDLE:
		down_states[MB_MIDDLE] = false;
		release_states[MB_RIGHT] = true;
		break;
	}
}

void
mouse_post_update(void)
{
	press_states[MB_LEFT] = release_states[MB_LEFT] = false;
	press_states[MB_RIGHT] = release_states[MB_RIGHT] = false;
	press_states[MB_MIDDLE] = release_states[MB_MIDDLE] = false;
}

bool
mouse_down(mouse_button_t b)
{
	return down_states[b];
}

bool
mouse_pressed(mouse_button_t b)
{
	return press_states[b];
}

bool
mouse_released(mouse_button_t b)
{
	return release_states[b];
}

void
mouse_pos(int *out_x, int *out_y)
{
	SDL_GetMouseState(out_x, out_y);
}
