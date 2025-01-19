#ifndef WND_H
#define WND_H

#include <SDL2/SDL.h>

#include "util.h"

extern SDL_Window *g_Wnd;
extern SDL_Renderer *g_Rend;

i32 Wnd_Init(void);
void Wnd_Quit(void);

#endif
