#include "wnd.h"

#include <stddef.h>
#include <stdlib.h>

#include "conf.h"
#include "util.h"

SDL_Window *g_wnd = NULL;
SDL_Renderer *g_rend = NULL;

int
wnd_init(void)
{
	atexit(wnd_quit);
	
	g_wnd = SDL_CreateWindow(CONF_WND_TITLE,
	                         SDL_WINDOWPOS_UNDEFINED,
	                         SDL_WINDOWPOS_UNDEFINED,
	                         CONF_WND_WIDTH,
	                         CONF_WND_HEIGHT,
	                         CONF_WND_FLAGS);
	if (!g_wnd)
	{
		log_err("wnd: failed to create window: %s\n", SDL_GetError());
		return 1;
	}
	
	g_rend = SDL_CreateRenderer(g_wnd, -1, CONF_REND_FLAGS);
	if (!g_rend)
	{
		log_err("wnd: failed to create renderer: %s\n"), SDL_GetError();
		return 1;
	}
	
	SDL_SetRenderDrawBlendMode(g_rend, SDL_BLENDMODE_BLEND);
	
	return 0;
}

void
wnd_quit(void)
{
	if (g_rend)
	{
		SDL_DestroyRenderer(g_rend);
		g_rend = NULL;
	}
	
	if (g_wnd)
	{
		SDL_DestroyWindow(g_wnd);
		g_wnd = NULL;
	}
}
