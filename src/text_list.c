#include "text_list.h"

#include <stddef.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "options.h"
#include "text.h"

#define QUEUED_MAX 64
#define TEXT_TICKS 550

static void GenBoxText(enum TextListItem Item);

static char BoxText[512];

static enum TextListItem Queue[QUEUED_MAX];
static unsigned QueueFrontTicks = 0;
static size_t QueueLen = 0;

void
TextList_Term(void)
{
	QueueFrontTicks = 0;
	QueueLen = 0;
}

void
TextList_Enqueue(enum TextListItem Item)
{
	if (QueueLen < QUEUED_MAX)
	{
		if (QueueLen == 0)
		{
			GenBoxText(Item);
			QueueFrontTicks = TEXT_TICKS;
		}
		
		Queue[QueueLen++] = Item;
	}
}

void
TextList_Update(void)
{
	if (QueueLen == 0)
		return;
	
	if (QueueFrontTicks == 0)
	{
		// remove first element and finish if nothing left.
		{
			memmove(
				&Queue[0],
				&Queue[1],
				sizeof(enum TextListItem) * (QUEUED_MAX - 1)
			);
			
			--QueueLen;
			if (QueueLen == 0)
				return;
		}
		
		// set new textbox parameters.
		{
			GenBoxText(Queue[0]);
			QueueFrontTicks = TEXT_TICKS;
		}
		
		return;
	}
	
	--QueueFrontTicks;
}

void
TextList_Draw(void)
{
	if (QueueFrontTicks > 0)
		Text_BoxDraw(BoxText);
}

static void
GenBoxText(enum TextListItem Item)
{
	switch (Item)
	{
	case TLI_CTE0_TEST:
		sprintf(BoxText, "Hello world");
		break;
		
		// c0e0.
	case TLI_C0E0_HOW_TO_MOVE:
		sprintf(BoxText, "Move left and right using [%s] and [%s]", SDL_GetKeyName(g_Options.KLeft), SDL_GetKeyName(g_Options.KRight));
		break;
	case TLI_C0E0_HOW_TO_JUMP:
		sprintf(BoxText, "Jump using [%s]", SDL_GetKeyName(g_Options.KJump));
		break;
	case TLI_C0E0_HOW_TO_WALLJUMP:
		sprintf(BoxText, "Walljump by jumping while in contact with a wall");
		break;
	case TLI_C0E0_HOW_TO_CLIMB:
		sprintf(BoxText, "Climb vertical corridors by chaining walljumps into eachother");
		break;
	case TLI_C0E0_HOW_TO_SLIDE:
		sprintf(BoxText, "Slide down walls by moving into them while falling");
		break;
	case TLI_C0E0_HOW_TO_WIN:
		sprintf(BoxText, "Finish the level by entering the yellow portal");
		break;
		
		// c0e1.
	case TLI_C0E1_KILL_INTRO:
		sprintf(BoxText, "Don't fall into the kill tiles!");
		break;
	case TLI_C0E1_MOMENTUM_INTRO:
		sprintf(BoxText, "Use the momentum from a prior jump to jump over the kill tiles");
		break;
	case TLI_C0E1_HOW_TO_POWERJUMP:
		sprintf(BoxText, "Powerjump in the direction you're moving using [%s]", SDL_GetKeyName(g_Options.KPowerjump));
		break;
		
		// c0e2.
	case TLI_C0E2_BOUNCE_INTRO:
		sprintf(BoxText, "Bounce tiles' restitution is proportional to the velocity at which you hit them");
		break;
		
		// c0e3.
	case TLI_C0E3_LEAP_OF_FAITH:
		sprintf(BoxText, "Leap of faith!");
		break;
		
		// c0e4.
	case TLI_C0E4_HOW_TO_AIR_CONTROL:
		sprintf(BoxText, "Move in the opposite direction of your velocity to slow down mid-air");
		break;
	case TLI_C0E4_LAUNCH_INTRO:
		sprintf(BoxText, "Launch tiles give you a sudden boost of velocity");
		break;
		
		// c0e5.
	case TLI_C0E5_HORIZONTAL_LAUNCH_INTRO:
		sprintf(BoxText, "Hitting a launch tile from the side will give you a boost of horizontal velocity");
		break;
		
		// c0e6.
	case TLI_C0E6_END_OFF_INTRO:
		sprintf(BoxText, "Wait, the end portal is disabled?");
		break;
	case TLI_C0E6_SWITCH_INTRO:
		sprintf(BoxText, "Try hitting those switch tiles...");
		break;
		
		// c0e8.
	case TLI_C0E8_HOW_TO_DASH_DOWN:
		sprintf(BoxText, "Dash downwards using [%s]", SDL_GetKeyName(g_Options.KDashDown));
		break;
	case TLI_C0E8_EXPLOIT_RESTITUTION:
		sprintf(BoxText, "Try exploiting the restitution of the bounce tiles to get somewhere you shouldn't be able to");
		break;
	case TLI_C0E8_DODGE_OBSTACLES:
		sprintf(BoxText, "Dodge obstacles by rapidly changing your vertical velocity");
		break;
		
		// c0e9.
	case TLI_C0E9_SLIPPERY_INTRO:
		sprintf(BoxText, "You can't get a proper grip on slippery tiles");
		break;
		
		// c0e11.
	case TLI_C0E11_GRIP_INTRO:
		sprintf(BoxText, "Grip tiles are rougher than normal, allowing you to climb up them");
		break;
		
		// ignore.
	case TLI_END__:
		break;
	}
}
