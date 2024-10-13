#include "text_list.h"

#include <stddef.h>
#include <string.h>

#include <SDL.h>

#include "conf.h"
#include "text.h"

#define QUEUED_MAX 64

static void gen_box_text(text_list_item_t item);

static char box_text[512];

// when creating a text box with the text list item, the tick duration for the
// visibility of the box should be looked up from this LUT.
static unsigned box_ticks_lut[] =
{
	[TLI_CTE0_TEST] = 250,
	
	[TLI_C0E0_HOW_TO_MOVE] = 250,
	[TLI_C0E0_HOW_TO_JUMP] = 250,
	[TLI_C0E0_HOW_TO_WALLJUMP] = 250,
	[TLI_C0E0_HOW_TO_CLIMB] = 250,
	[TLI_C0E0_HOW_TO_SLIDE] = 250,
	[TLI_C0E0_HOW_TO_WIN] = 250,
	
	[TLI_C0E1_KILL_INTRO] = 250,
	[TLI_C0E1_MOMENTUM_INTRO] = 250,
	[TLI_C0E1_HOW_TO_POWERJUMP] = 250,
	[TLI_C0E1_HOW_TO_FALL] = 250,
};

static text_list_item_t queue[QUEUED_MAX];
static unsigned queue_front_ticks = 0;
static size_t nqueued = 0;

void
text_list_enqueue(text_list_item_t item)
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
			        sizeof(text_list_item_t) * (QUEUED_MAX - 1));
			
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
gen_box_text(text_list_item_t item)
{
	switch (item)
	{
	case TLI_CTE0_TEST:
		sprintf(box_text, "Hello world");
		break;
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
	case TLI_C0E1_KILL_INTRO:
		sprintf(box_text, "Don't fall into the kill blocks!");
		break;
	case TLI_C0E1_MOMENTUM_INTRO:
		sprintf(box_text, "Use the momentum from a prior jump to jump over the kill blocks");
		break;
	case TLI_C0E1_HOW_TO_POWERJUMP:
		sprintf(box_text, "Powerjump in the direction you're moving using [%s]", SDL_GetKeyName(CONF_KEY_POWERJUMP));
		break;
	case TLI_C0E1_HOW_TO_FALL:
		sprintf(box_text, "Fall faster by holding [%s] in the air", SDL_GetKeyName(CONF_KEY_FALL));
		break;
	}
}
