#include "text_list.h"

#include <string.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "options.h"
#include "sound.h"
#include "text.h"
#include "util.h"

#define QUEUED_MAX 64
#define TEXT_TICKS 550

static void GenBoxText(enum TextListItem Item);

static char BoxText[512];

static enum TextListItem Queue[QUEUED_MAX];
static u32 QueueFrontTicks = 0;
static u32 QueueLen = 0;

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
			Sound_PlaySfx(SI_TEXTBOX);
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
			Sound_PlaySfx(SI_TEXTBOX);
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
	case TLI_TEST:
		sprintf(BoxText, "Hello world");
		break;
	case TLI_HOW_TO_MOVE:
	{
		char LeftName[32], RightName[32];
		sprintf(LeftName, "%s", SDL_GetKeyName(g_Options.KLeft));
		sprintf(RightName, "%s", SDL_GetKeyName(g_Options.KRight));
		sprintf(BoxText, "Move left and right using [%s] and [%s]", LeftName, RightName);
		break;
	}
	case TLI_HOW_TO_JUMP:
		sprintf(BoxText, "Jump using [%s]", SDL_GetKeyName(g_Options.KJump));
		break;
	case TLI_HOW_TO_WALLJUMP:
		sprintf(BoxText, "Walljump by jumping while in contact with a wall");
		break;
	case TLI_HOW_TO_CLIMB:
		sprintf(BoxText, "Climb vertical corridors by chaining walljumps into eachother");
		break;
	case TLI_HOW_TO_SLIDE:
		sprintf(BoxText, "Slide down walls by moving into them while falling");
		break;
	case TLI_HOW_TO_WIN:
		sprintf(BoxText, "Finish the level by entering the yellow portal");
		break;
	case TLI_KILL_INTRO:
		sprintf(BoxText, "Don't fall into the kill tiles!");
		break;
	case TLI_MOMENTUM_INTRO:
		sprintf(BoxText, "Use the momentum from a prior jump to jump over the kill tiles");
		break;
	case TLI_HOW_TO_POWERJUMP:
		sprintf(BoxText, "Powerjump in the direction you're moving using [%s]", SDL_GetKeyName(g_Options.KPowerjump));
		break;
	case TLI_BOUNCE_INTRO:
		sprintf(BoxText, "Bounce tiles' restitution is proportional to the velocity at which you hit them");
		break;
	case TLI_LEAP_OF_FAITH:
		sprintf(BoxText, "Leap of faith!");
		break;
	case TLI_BOUNCE_MULTIPLE:
		sprintf(BoxText, "Bounce multiple times to reduce vertical bounce velocity over time");
		break;
	case TLI_LAUNCH_INTRO:
		sprintf(BoxText, "Launch tiles give you a sudden boost of velocity");
		break;
	case TLI_HORIZONTAL_LAUNCH_INTRO:
		sprintf(BoxText, "Hitting a launch tile from the side will give you a boost of horizontal velocity");
		break;
	case TLI_END_OFF_INTRO:
		sprintf(BoxText, "Wait, the end portal is disabled?");
		break;
	case TLI_SWITCH_INTRO:
		sprintf(BoxText, "Try hitting those switch tiles...");
		break;
	case TLI_HOW_TO_DASH_DOWN:
		sprintf(BoxText, "Dash downwards using [%s]", SDL_GetKeyName(g_Options.KDashDown));
		break;
	case TLI_EXPLOIT_RESTITUTION:
		sprintf(BoxText, "Try exploiting the restitution of the bounce tiles to get somewhere you shouldn't be able to");
		break;
	case TLI_DODGE_OBSTACLES:
		sprintf(BoxText, "Dodge obstacles by rapidly changing your vertical velocity");
		break;
	case TLI_SLIPPERY_INTRO:
		sprintf(BoxText, "You can't get a proper grip on slippery tiles");
		break;
	case TLI_GRIP_INTRO:
		sprintf(BoxText, "Grip tiles are rougher than normal, allowing you to climb up them");
		break;
	case TLI_TERMINATE_JUMP:
		sprintf(BoxText, "Dash downwards while powerjumping to jump on platforms you would otherwise miss");
		break;
	case TLI_BEAM_INTRO:
		sprintf(BoxText, "Beam tiles will suck you in towards them at a constant speed");
		break;
	case TLI_FALL_DIRECTIONS:
		sprintf(BoxText, "Right, center, left, center, bounce!");
		break;
	default:
		break;
	}
}
