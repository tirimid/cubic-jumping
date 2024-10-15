#ifndef MENUS_H
#define MENUS_H

// used for communication between menus and sometimes their callers.
typedef enum menu_request
{
	MR_NONE = 0,
	MR_SELECT,
	MR_NEXT,
	MR_RETRY,
} menu_request;

void main_menu_loop(void);
char const *custom_level_select_menu_loop(void);
menu_request level_end_menu_loop(void);
void pause_menu_loop(void);

#endif
