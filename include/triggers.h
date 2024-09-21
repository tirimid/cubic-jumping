#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdint.h>

#define TRIGGERS_MAX 64

typedef enum trigger_type
{
	// not *really* a trigger type.
	// used as a mechanism to allow a trigger erase feature.
	TT_NULL = 0,
	
	TT_MSG,
	TT_KILL,
	
	TT_END__,
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

uint8_t const *trigger_color(trigger_type_t type);
void triggers_add_trigger(trigger_type_t type, float px, float py, float sx, float sy);
void triggers_update(void);
void triggers_draw(void);

#endif
