#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#include <SDL.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void log_err(char const *fmt, ...);
uint64_t get_unix_time_ms(void);
float lerp(float a, float b, float t);
void relative_draw_rect(SDL_Renderer *rend, float x, float y, float w, float h);

#endif
