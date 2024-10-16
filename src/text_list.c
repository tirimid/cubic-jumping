#include "text_list.h"

#include <stddef.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "options.h"
#include "text.h"

#define QUEUED_MAX 64
#define TEXT_TICKS 500

static void gen_box_text(text_list_item item);

static char box_text[512];

static text_list_item queue[QUEUED_MAX];
static unsigned queue_front_ticks = 0;
static size_t nqueued = 0;

void
text_list_term(void)
{
	queue_front_ticks = 0;
	nqueued = 0;
}

void
text_list_enqueue(text_list_item item)
{
	if (nqueued < QUEUED_MAX)
	{
		if (nqueued == 0)
		{
			gen_box_text(item);
			queue_front_ticks = TEXT_TICKS;
		}
		
		queue[nqueued++] = item;
	}
}

void
text_list_update(void)
{
	if (nqueued == 0)
		return;
	
	if (queue_front_ticks == 0)
	{
		// remove first element and finish if nothing left.
		{
			memmove(&queue[0],
			        &queue[1],
			        sizeof(text_list_item) * (QUEUED_MAX - 1));
			
			--nqueued;
			if (nqueued == 0)
				return;
		}
		
		// set new textbox parameters.
		{
			gen_box_text(queue[0]);
			queue_front_ticks = TEXT_TICKS;
		}
		
		return;
	}
	
	--queue_front_ticks;
}

void
text_list_draw(void)
{
	if (nqueued > 0)
		text_box_draw(box_text);
}

static void
gen_box_text(text_list_item item)
{
	switch (item)
	{
	case TLI_CTE0_TEST:
		sprintf(box_text, "Hello world");
		break;
		
		// c0e0.
	case TLI_C0E0_HOW_TO_MOVE:
		sprintf(box_text, "Move left and right using [%s] and [%s]", SDL_GetKeyName(g_options.k_left), SDL_GetKeyName(g_options.k_right));
		break;
	case TLI_C0E0_HOW_TO_JUMP:
		sprintf(box_text, "Jump using [%s]", SDL_GetKeyName(g_options.k_jump));
		break;
	case TLI_C0E0_HOW_TO_WALLJUMP:
		sprintf(box_text, "Walljump by jumping while in contact with a wall");
		break;
	case TLI_C0E0_HOW_TO_CLIMB:
		sprintf(box_text, "Climb vertical corridors by chaining walljumps into eachother");
		break;
	case TLI_C0E0_HOW_TO_SLIDE:
		sprintf(box_text, "Slide down walls by moving into them while falling");
		break;
	case TLI_C0E0_HOW_TO_WIN:
		sprintf(box_text, "Finish the level by entering the yellow portal");
		break;
		
		// c0e1.
	case TLI_C0E1_KILL_INTRO:
		sprintf(box_text, "Don't fall into the kill tiles!");
		break;
	case TLI_C0E1_MOMENTUM_INTRO:
		sprintf(box_text, "Use the momentum from a prior jump to jump over the kill tiles");
		break;
	case TLI_C0E1_HOW_TO_POWERJUMP:
		sprintf(box_text, "Powerjump in the direction you're moving using [%s]", SDL_GetKeyName(g_options.k_powerjump));
		break;
		
		// c0e2.
	case TLI_C0E2_BOUNCE_INTRO:
		sprintf(box_text, "Bounce tiles' restitution is proportional to the velocity at which you hit them");
		break;
		
		// c0e3.
	case TLI_C0E3_LEAP_OF_FAITH:
		sprintf(box_text, "Leap of faith!");
		break;
		
		// c0e4.
	case TLI_C0E4_HOW_TO_AIR_CONTROL:
		sprintf(box_text, "Move in the opposite direction of your velocity to slow down mid-air");
		break;
	case TLI_C0E4_LAUNCH_INTRO:
		sprintf(box_text, "Launch tiles give you a sudden boost of velocity");
		break;
		
		// c0e5.
	case TLI_C0E5_HORIZONTAL_LAUNCH_INTRO:
		sprintf(box_text, "Hitting a launch tile from the side will give you a boost of horizontal velocity");
		break;
		
		// c0e6.
	case TLI_C0E6_END_OFF_INTRO:
		sprintf(box_text, "Wait, the end portal is disabled?");
		break;
	case TLI_C0E6_SWITCH_INTRO:
		sprintf(box_text, "Try hitting those switch tiles...");
		break;
		
		// c0e8.
	case TLI_C0E8_HOW_TO_DASH_DOWN:
		sprintf(box_text, "Dash downwards using [%s]", SDL_GetKeyName(g_options.k_dash_down));
		break;
	case TLI_C0E8_EXPLOIT_RESTITUTION:
		sprintf(box_text, "Try exploiting the restitution of the bounce tiles to get somewhere you shouldn't be able to");
		break;
	case TLI_C0E8_DODGE_OBSTACLES:
		sprintf(box_text, "Dodge obstacles by rapidly changing your vertical velocity");
		break;
	}
}
