#ifndef TEXT_LIST_H
#define TEXT_LIST_H

typedef enum text_list_item
{
	TLI_CTE0_TEST = 0,
	TLI_C0E0_MOVE_LEFT_RIGHT,
} text_list_item_t;

// when creating a text box with the text list item, the tick duration for the
// visibility of the box should be looked up from this LUT.
extern unsigned text_list_ticks_lut[];

char const *text_list_gen_msg(text_list_item_t item);

#endif
