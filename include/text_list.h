#ifndef TEXT_LIST_H
#define TEXT_LIST_H

typedef enum text_list_item
{
	TLI_CTE0_TEST = 0,
	TLI_C0E0_MOVE_LEFT_RIGHT,
	
	TLI_END__,
} text_list_item_t;

void text_list_enqueue(text_list_item_t item);
void text_list_update(void);
void text_list_draw(void);

#endif
