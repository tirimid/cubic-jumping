#include "wnd.h"

#include <stdlib.h>

#include "conf.h"

SDL_Window *g_Wnd = NULL;
SDL_Renderer *g_Rend = NULL;

i32
Wnd_Init(void)
{
	atexit(Wnd_Quit);
	
	g_Wnd = SDL_CreateWindow(
		CONF_WND_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		CONF_WND_WIDTH,
		CONF_WND_HEIGHT,
		CONF_WND_FLAGS
	);
	
	if (!g_Wnd)
	{
		LogErr("wnd: failed to create window: %s", SDL_GetError());
		return 1;
	}
	
	g_Rend = SDL_CreateRenderer(g_Wnd, -1, CONF_REND_FLAGS);
	if (!g_Rend)
	{
		LogErr("wnd: failed to create renderer: %s"), SDL_GetError();
		return 1;
	}
	
	SDL_SetRenderDrawBlendMode(g_Rend, SDL_BLENDMODE_BLEND);
	
	return 0;
}

void
Wnd_Quit(void)
{
	if (g_Rend)
	{
		SDL_DestroyRenderer(g_Rend);
		g_Rend = NULL;
	}
	
	if (g_Wnd)
	{
		SDL_DestroyWindow(g_Wnd);
		g_Wnd = NULL;
	}
}
