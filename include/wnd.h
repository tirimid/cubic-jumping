#ifndef WND_H
#define WND_H

#include <SDL2/SDL.h>

extern SDL_Window *g_wnd;
extern SDL_Renderer *g_rend;

int wnd_init(void);
void wnd_quit(void);

#endif
