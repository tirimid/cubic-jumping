#include "util.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <sys/time.h>

#include "cam.h"
#include "conf.h"
#include "wnd.h"

#define ERR_TITLE "CJ - Error"
#define MAX_LOG_LEN 512

void
LogErr(char const *Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	
	char Msg[MAX_LOG_LEN];
	vsnprintf(Msg, MAX_LOG_LEN, Fmt, Args);
	
	// stderr is backup so that error can still be processed in case a message
	// box can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, ERR_TITLE, Msg, NULL))
		fprintf(stderr, "%s\n", Msg);
	
	va_end(Args);
}

u64
GetUnixTimeMs(void)
{
	struct timeval Tv;
	gettimeofday(&Tv, NULL);
	return (u64)Tv.tv_sec * 1000 + (u64)Tv.tv_usec / 1000;
}

f32
Lerp(f32 a, f32 b, f32 t)
{
	return a + t * (b - a);
}

void
RelativeDrawRect(f32 x, f32 y, f32 w, f32 h)
{
	// 1 is added to pixel width and height in order to remove any seams that
	// appear as a result of dynamic camera movement / zooming.
	SDL_Rect Rect =
	{
		.w = g_Cam.Zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_Cam.Zoom * CONF_DRAW_SCALE * h + 1
	};
	GameToScreenCoord(&Rect.x, &Rect.y, x, y);
	SDL_RenderFillRect(g_Rend, &Rect);
}

void
RelativeDrawHollowRect(f32 x, f32 y, f32 w, f32 h)
{
	SDL_Rect Rect =
	{
		.w = g_Cam.Zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_Cam.Zoom * CONF_DRAW_SCALE * h + 1
	};
	GameToScreenCoord(&Rect.x, &Rect.y, x, y);
	SDL_RenderDrawRect(g_Rend, &Rect);
}

f32
RandFloat(f32 Max)
{
	return (f32)rand() / (f32)(RAND_MAX / Max);
}

i32
RandInt(i32 Max)
{
	return rand() % Max;
}

void
GameToScreenCoord(i32 *OutX, i32 *OutY, f32 x, f32 y)
{
	*OutX = g_Cam.Zoom * CONF_DRAW_SCALE * (x - g_Cam.PosX) + CONF_WND_WIDTH / 2;
	*OutY = g_Cam.Zoom * CONF_DRAW_SCALE * (y - g_Cam.PosY) + CONF_WND_HEIGHT / 2;
}

void
ScreenToGameCoord(f32 *OutX, f32 *OutY, i32 x, i32 y)
{
	*OutX = (x - CONF_WND_WIDTH / 2) / (g_Cam.Zoom * CONF_DRAW_SCALE) + g_Cam.PosX;
	*OutY = (y - CONF_WND_HEIGHT / 2) / (g_Cam.Zoom * CONF_DRAW_SCALE) + g_Cam.PosY;
}

u32
CountLines(char const *s)
{
	u32 n = 1;
	for (usize i = 0; s[i]; ++i)
		n += s[i] == '\n';
	return n;
}
