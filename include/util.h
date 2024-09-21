#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void log_err(char const *fmt, ...);
uint64_t get_unix_time_ms(void);
float lerp(float a, float b, float t);
void relative_draw_rect(float x, float y, float w, float h);
void relative_draw_hollow_rect(float x, float y, float w, float h);
float rand_float(float max); // ranges from [0, max].
int rand_int(int max); // ranges from [0, max).
void game_to_screen_coord(int *out_x, int *out_y, float x, float y);
void screen_to_game_coord(float *out_x, float *out_y, int x, int y);

#endif
