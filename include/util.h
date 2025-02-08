#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(Min, n, Max) MIN((Max), MAX((n), (Min)))

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef float f32;
typedef double f64;

void LogErr(char const *Fmt, ...);
u64 GetUnixTimeMs(void);
f32 Lerp(f32 a, f32 b, f32 t);
void RelativeDrawRect(f32 x, f32 y, f32 w, f32 h);
void RelativeDrawHollowRect(f32 x, f32 y, f32 w, f32 h);
f32 RandFloat(f32 Max); // ranges from [0, Max].
i32 RandInt(i32 Max); // ranges from [0, Max).
void GameToScreenCoord(i32 *OutX, i32 *OutY, f32 x, f32 y);
void ScreenToGameCoord(f32 *OutX, f32 *OutY, i32 x, i32 y);
u32 CountLines(char const *s);

#endif
