#ifndef TEXT_LIST_H
#define TEXT_LIST_H

// consult these values when placing message triggers in maps.
typedef enum text_list_item
{
	TLI_CTE0_TEST = 0x0,
	
	TLI_C0E0_HOW_TO_MOVE = 0x1,
	TLI_C0E0_HOW_TO_JUMP = 0x2,
	TLI_C0E0_HOW_TO_WALLJUMP = 0x3,
	TLI_C0E0_HOW_TO_CLIMB = 0x4,
	TLI_C0E0_HOW_TO_SLIDE = 0x5,
	TLI_C0E0_HOW_TO_WIN = 0x6,
	
	TLI_C0E1_KILL_INTRO = 0x7,
	TLI_C0E1_MOMENTUM_INTRO = 0x8,
	TLI_C0E1_HOW_TO_POWERJUMP = 0x9,
	TLI_C0E1_HOW_TO_FALL = 0xa,
	
	TLI_END__,
} text_list_item_t;

void text_list_enqueue(text_list_item_t item);
void text_list_update(void);
void text_list_draw(void);

#endif
