#include "util.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <sys/time.h>

#include "cam.h"
#include "conf.h"

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
		fprintf(stderr, msg);
	
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
relative_draw_rect(SDL_Renderer *rend, float x, float y, float w, float h)
{
	// 1 is added to pixel width and height in order to remove any seams that
	// appear as a result of dynamic camera movement / zooming.
	SDL_Rect rect =
	{
		.x = g_cam.zoom * CONF_DRAW_SCALE * (x - g_cam.pos_x) + CONF_WND_WIDTH / 2,
		.y = g_cam.zoom * CONF_DRAW_SCALE * (y - g_cam.pos_y) + CONF_WND_HEIGHT / 2,
		.w = g_cam.zoom * CONF_DRAW_SCALE * w + 1,
		.h = g_cam.zoom * CONF_DRAW_SCALE * h + 1,
	};
	SDL_RenderFillRect(rend, &rect);
}
