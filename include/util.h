#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(Min, n, Max) MIN((Max), MAX((n), (Min)))

void LogErr(char const *Fmt, ...);
uint64_t GetUnixTimeMs(void);
float Lerp(float a, float b, float t);
void RelativeDrawRect(float x, float y, float w, float h);
void RelativeDrawHollowRect(float x, float y, float w, float h);
float RandFloat(float Max); // ranges from [0, max].
int RandInt(int Max); // ranges from [0, max).
void GameToScreenCoord(int *OutX, int *OutY, float x, float y);
void ScreenToGameCoord(float *OutX, float *OutY, int x, int y);
unsigned CountLines(char const *s);

#endif
