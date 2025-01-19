#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
	float PosX, PosY;
	float SizeX, SizeY;
	uint32_t Arg; // type-dependent argument.
	bool SingleUse;
	uint8_t Type;
};

extern struct Trigger g_Triggers[TRIGGERS_MAX];
extern size_t g_TriggerCnt;

uint8_t const *Trigger_TypeColor(enum TriggerType Type);
void Triggers_AddTrigger(struct Trigger const *Trigger);
void Triggers_RmTrigger(size_t Idx);
void Triggers_Update(void);
void Triggers_Draw(void);

#endif
