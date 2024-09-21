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
	200,
	400,
	200,
	400,
	500,
	400,
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
		do
		{
			memmove(&queue[0],
			        &queue[1],
			        sizeof(text_list_item_t) * (QUEUED_MAX - 1));
			
			--nqueued;
			if (nqueued == 0)
				return;
		} while (0);
		
		// set new textbox parameters.
		do
		{
			gen_box_text(queue[0]);
			queue_front_ticks = box_ticks_lut[queue[0]];
		} while (0);
		
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
		sprintf(box_text, "Move left and right using <%s> and <%s>", SDL_GetKeyName(CONF_KEY_LEFT), SDL_GetKeyName(CONF_KEY_RIGHT));
		break;
	case TLI_C0E0_HOW_TO_JUMP:
		sprintf(box_text, "Jump using <%s>", SDL_GetKeyName(CONF_KEY_JUMP));
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
	}
}
