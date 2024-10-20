#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TRIGGERS_MAX 64

enum trigger_type
{
	TT_MSG = 0,
	TT_KILL,
	TT_MSG_TERM,
	TT_CAP_ENABLE,
	TT_CAP_DISABLE,
	
	TT_END__,
};

struct trigger
{
	float pos_x, pos_y;
	float size_x, size_y;
	uint32_t arg; // type-dependent argument.
	bool single_use;
	uint8_t type;
};

extern struct trigger g_triggers[TRIGGERS_MAX];
extern size_t g_ntriggers;

uint8_t const *trigger_color(enum trigger_type type);
void triggers_add_trigger(struct trigger const *trigger);
void triggers_rm_trigger(size_t ind);
void triggers_update(void);
void triggers_draw(void);

#endif
