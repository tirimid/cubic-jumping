#include "text_list.h"

#include <string.h>

#include <SDL.h>

#include "conf.h"

unsigned text_list_ticks_lut[] =
{
	300,
	500,
};

char const *
text_list_gen_msg(text_list_item_t item)
{
	static char buf[512] = {0};
	
	switch (item)
	{
	case TLI_CTE0_TEST:
		sprintf(buf, "Hello world");
		break;
	case TLI_C0E0_MOVE_LEFT_RIGHT:
		sprintf(buf, "Move left and right using <%s> and <%s>", SDL_GetKeyName(CONF_KEY_LEFT), SDL_GetKeyName(CONF_KEY_RIGHT));
		break;
	}
	
	return buf;
}
