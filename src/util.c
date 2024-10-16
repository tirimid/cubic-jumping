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

#define ERR_TITLE "CJ - error"
#define MAX_LOG_LEN 512

void
log_err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[MAX_LOG_LEN];
	vsnprintf(msg, MAX_LOG_LEN, fmt, args);
	
	// stderr is backup so that error can still be processed in case a message
	// box can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, ERR_TITLE, msg, NULL))
		fprintf(stderr, "%s\n", msg);
	
	va_end(args);
}

uint64_t
get_unix_time_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

float
lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

void
relative_draw_rect(float x, float y, float w, float h)
{
	// 1 is added to pixel width and height in order to remove any seams that
	// appear as a result of dynamic camera movement / zooming.
	SDL_Rect rect =
	{
		.w = g_cam.zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_cam.zoom * CONF_DRAW_SCALE * h + 1,
	};
	game_to_screen_coord(&rect.x, &rect.y, x, y);
	SDL_RenderFillRect(g_rend, &rect);
}

void
relative_draw_hollow_rect(float x, float y, float w, float h)
{
	SDL_Rect rect =
	{
		.w = g_cam.zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_cam.zoom * CONF_DRAW_SCALE * h + 1,
	};
	game_to_screen_coord(&rect.x, &rect.y, x, y);
	SDL_RenderDrawRect(g_rend, &rect);
}

float
rand_float(float max)
{
	return (float)rand() / (float)(RAND_MAX / max);
}

int
rand_int(int max)
{
	return rand() % max;
}

void
game_to_screen_coord(int *out_x, int *out_y, float x, float y)
{
	*out_x = g_cam.zoom * CONF_DRAW_SCALE * (x - g_cam.pos_x) + CONF_WND_WIDTH / 2;
	*out_y = g_cam.zoom * CONF_DRAW_SCALE * (y - g_cam.pos_y) + CONF_WND_HEIGHT / 2;
}

void
screen_to_game_coord(float *out_x, float *out_y, int x, int y)
{
	*out_x = (x - CONF_WND_WIDTH / 2) / (g_cam.zoom * CONF_DRAW_SCALE) + g_cam.pos_x;
	*out_y = (y - CONF_WND_HEIGHT / 2) / (g_cam.zoom * CONF_DRAW_SCALE) + g_cam.pos_y;
}

unsigned
count_lines(char const *s)
{
	unsigned n = 1;
	for (size_t i = 0; s[i]; ++i)
		n += s[i] == '\n';
	return n;
}
