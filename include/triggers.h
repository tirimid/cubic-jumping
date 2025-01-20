#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdbool.h>

#include "conf.h"
#include "util.h"

#define TRIGGERS_MAX 64

enum TriggerType
{
	TT_MSG = 0,
	TT_KILL,
	TT_MSG_TERM,
	TT_CAP_ENABLE,
	TT_CAP_DISABLE,
	
	TT_END__
};

struct Trigger
{
	f32 PosX, PosY;
	f32 SizeX, SizeY;
	u32 Arg; // type-dependent argument.
	bool SingleUse;
	u8 Type;
};

extern struct Trigger g_Triggers[TRIGGERS_MAX];
extern usize g_TriggerCnt;

// tables.
extern u8 Trigger_Color[TT_END__][3];

void Triggers_AddTrigger(struct Trigger const *Trigger);
void Triggers_RmTrigger(usize Idx);
void Triggers_Update(void);
void Triggers_Draw(void);

#endif
