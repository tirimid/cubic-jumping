#ifndef MENUS_H
#define MENUS_H

// menu requests allow the menus to demand something of their caller.
// e.g. they can request `MR_EXIT` to make the caller exit to the main menu or
// something.
typedef enum menu_request
{
	MR_NONE = 0,
	MR_EXIT,
	MR_NEXT,
	MR_RETRY,
} menu_request;

void main_menu_loop(void);
menu_request level_end_menu_loop(void);
menu_request pause_menu_loop(void);

#endif
