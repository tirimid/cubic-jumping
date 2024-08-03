#ifndef MAP_LIST_H
#define MAP_LIST_H

typedef enum map_list_item
{
	MLI_CTE0 = 0,
	
	MLI_END__,
} map_list_item_t;

map_list_item_t map_list_cur_item(void);
void map_list_load(map_list_item_t item);

#endif
