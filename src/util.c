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

uint64_t
GetUnixTimeMs(void)
{
	struct timeval Tv;
	gettimeofday(&Tv, NULL);
	return (uint64_t)Tv.tv_sec * 1000 + (uint64_t)Tv.tv_usec / 1000;
}

float
Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

void
RelativeDrawRect(float x, float y, float w, float h)
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
RelativeDrawHollowRect(float x, float y, float w, float h)
{
	SDL_Rect Rect =
	{
		.w = g_Cam.Zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_Cam.Zoom * CONF_DRAW_SCALE * h + 1
	};
	GameToScreenCoord(&Rect.x, &Rect.y, x, y);
	SDL_RenderDrawRect(g_Rend, &Rect);
}

float
RandFloat(float Max)
{
	return (float)rand() / (float)(RAND_MAX / Max);
}

int
RandInt(int Max)
{
	return rand() % Max;
}

void
GameToScreenCoord(int *OutX, int *OutY, float x, float y)
{
	*OutX = g_Cam.Zoom * CONF_DRAW_SCALE * (x - g_Cam.PosX) + CONF_WND_WIDTH / 2;
	*OutY = g_Cam.Zoom * CONF_DRAW_SCALE * (y - g_Cam.PosY) + CONF_WND_HEIGHT / 2;
}

void
ScreenToGameCoord(float *OutX, float *OutY, int x, int y)
{
	*OutX = (x - CONF_WND_WIDTH / 2) / (g_Cam.Zoom * CONF_DRAW_SCALE) + g_Cam.PosX;
	*OutY = (y - CONF_WND_HEIGHT / 2) / (g_Cam.Zoom * CONF_DRAW_SCALE) + g_Cam.PosY;
}

unsigned
CountLines(char const *s)
{
	unsigned n = 1;
	for (size_t i = 0; s[i]; ++i)
		n += s[i] == '\n';
	return n;
}
