#include "text_list.h"

#include <stddef.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "text.h"

#define QUEUED_MAX 64

static void gen_box_text(text_list_item item);

static char box_text[512];

// when creating a text box with the text list item, the tick duration for the
// visibility of the box should be looked up from this LUT.
static unsigned box_ticks_lut[] =
{
	[TLI_CTE0_TEST] = 500,
	
	// c0e0.
	[TLI_C0E0_HOW_TO_MOVE] = 500,
	[TLI_C0E0_HOW_TO_JUMP] = 500,
	[TLI_C0E0_HOW_TO_WALLJUMP] = 500,
	[TLI_C0E0_HOW_TO_CLIMB] = 500,
	[TLI_C0E0_HOW_TO_SLIDE] = 500,
	[TLI_C0E0_HOW_TO_WIN] = 500,
	
	// c0e1.
	[TLI_C0E1_KILL_INTRO] = 500,
	[TLI_C0E1_MOMENTUM_INTRO] = 500,
	[TLI_C0E1_HOW_TO_POWERJUMP] = 500,
	
	// c0e2.
	[TLI_C0E2_BOUNCE_INTRO] = 500,
	
	// c0e3.
	[TLI_C0E3_LEAP_OF_FAITH] = 500,
	
	// c0e4.
	[TLI_C0E4_HOW_TO_AIR_CONTROL] = 500,
	[TLI_C0E4_LAUNCH_INTRO] = 500,
};

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
			queue_front_ticks = box_ticks_lut[item];
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
			queue_front_ticks = box_ticks_lut[queue[0]];
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
		sprintf(box_text, "Move left and right using [%s] and [%s]", SDL_GetKeyName(CONF_KEY_LEFT), SDL_GetKeyName(CONF_KEY_RIGHT));
		break;
	case TLI_C0E0_HOW_TO_JUMP:
		sprintf(box_text, "Jump using [%s]", SDL_GetKeyName(CONF_KEY_JUMP));
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
		sprintf(box_text, "Powerjump in the direction you're moving using [%s]", SDL_GetKeyName(CONF_KEY_POWERJUMP));
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
	}
}
