#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdint.h>

#include <SDL.h>

#define TRIGGERS_MAX 64

typedef enum trigger_type
{
	TT_NEXT_LEVEL = 0,
	TT_TEXT_MSG,
} trigger_type_t;

typedef struct trigger
{
	float pos_x, pos_y;
	float size_x, size_y;
	int32_t uses_left; // -1 means infinite uses.
	uint32_t arg; // type-dependent argument.
	uint8_t type;
} trigger_t;

extern trigger_t g_triggers[TRIGGERS_MAX];
extern uint32_t g_ntriggers;

void triggers_add_trigger(trigger_type_t type, float px, float py, float sx, float sy);
void triggers_update(void);
void triggers_draw(SDL_Renderer *rend);

#endif
