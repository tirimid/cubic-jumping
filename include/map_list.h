#ifndef MAP_LIST_H
#define MAP_LIST_H

typedef enum map_list_item
{
	// chapter test.
	MLI_CTE0 = 0,
	MLI_CTE1,
	
	// chapter 0.
	MLI_C0E0,
	MLI_C0E1,
	
	MLI_END__,
} map_list_item_t;

void map_list_load(map_list_item_t item);
void map_list_reload(void);
void map_list_load_next(void);

#endif
