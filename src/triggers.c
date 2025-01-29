#include "triggers.h"

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "player.h"
#include "sound.h"
#include "text.h"
#include "text_list.h"
#include "util.h"
#include "wnd.h"

struct Trigger g_Triggers[TRIGGERS_MAX];
usize g_TriggerCnt = 0;

u8 Trigger_Color[TT_END__][3] =
{
	CONF_COLOR_TRIGGER_MSG,
	CONF_COLOR_TRIGGER_SWAP_REGION,
	CONF_COLOR_TRIGGER_MSG_TERM,
	CONF_COLOR_TRIGGER_CAP_ENABLE,
	CONF_COLOR_TRIGGER_CAP_DISABLE
};

static void Collide(struct Trigger const *Trigger, usize Idx);

void
Triggers_AddTrigger(struct Trigger const *Trigger)
{
	if (g_TriggerCnt < TRIGGERS_MAX)
		g_Triggers[g_TriggerCnt++] = *Trigger;
}

void
Triggers_RmTrigger(usize Idx)
{
	if (g_TriggerCnt == 0)
		return;
	
	memmove(
		&g_Triggers[Idx],
		&g_Triggers[Idx + 1],
		sizeof(struct Trigger) * (g_TriggerCnt - Idx - 1)
	);
	--g_TriggerCnt;
}

void
Triggers_Update(void)
{
	for (u32 i = 0; i < g_TriggerCnt; ++i)
	{
		if (g_Player.PosX + CONF_PLAYER_SIZE >= g_Triggers[i].PosX
			&& g_Player.PosX < g_Triggers[i].PosX + g_Triggers[i].SizeX
			&& g_Player.PosY + CONF_PLAYER_SIZE >= g_Triggers[i].PosY
			&& g_Player.PosY < g_Triggers[i].PosY + g_Triggers[i].SizeY)
		{
			Collide(&g_Triggers[i], i);
		}
	}
}

void
Triggers_Draw(void)
{
	for (u32 i = 0; i < g_TriggerCnt; ++i)
	{
		struct Trigger const *t = &g_Triggers[i];
		
		// draw bounding box.
		{
			u8 const *Col = &Trigger_Color[t->Type][0];
			SDL_SetRenderDrawColor(
				g_Rend,
				Col[0],
				Col[1],
				Col[2],
				CONF_COLOR_TRIGGER_OPACITY
			);
		
			RelativeDrawRect(t->PosX, t->PosY, t->SizeX, t->SizeY);
		}
		
		// draw argument text.
		{
			i32 ScrX, ScrY;
			GameToScreenCoord(&ScrX, &ScrY, t->PosX, t->PosY);
			
			static char Buf[32];
			sprintf(Buf, "%x", t->Arg);
			
			Text_DrawStr(Buf, ScrX, ScrY);
		}
	}
}

static void
Collide(struct Trigger const *Trigger, usize Idx)
{
	// perform trigger functionality.
	switch (Trigger->Type)
	{
	case TT_MSG:
		if (Trigger->Arg < TLI_END__)
			TextList_Enqueue(Trigger->Arg);
		break;
	case TT_SWAP_REGION:
		if (Game_CurrentTick() % CONF_SWAP_TICKS == 0)
			Sound_PlaySfx(SI_SWAP_SWAP);
		else if (Game_CurrentTick() % CONF_SWAP_BEAT_TICKS == 0)
			Sound_PlaySfx(SI_SWAP_BEAT);
		break;
	case TT_MSG_TERM:
		TextList_Term();
		break;
	case TT_CAP_ENABLE:
		Player_SetCapMask(Trigger->Arg, false);
		break;
	case TT_CAP_DISABLE:
		Player_SetCapMask(Trigger->Arg, true);
		break;
	default:
		break;
	}
	
	if (Trigger->SingleUse)
		Triggers_RmTrigger(Idx);
}
