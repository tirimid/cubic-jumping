#ifndef MAP_LIST_H
#define MAP_LIST_H

typedef enum map_list_item
{
	// custom map.
	MLI_CUSTOM = 0,
	
	// chapter 0.
	MLI_C0E0,
	MLI_C0E1,
	MLI_C0E2,
	MLI_C0E3,
	MLI_C0E4,
	MLI_C0E5,
	MLI_C0E6,
	MLI_C0E7,
	MLI_C0E8,
	MLI_C0E9,
	MLI_C0E10,
	MLI_C0E11,
	MLI_C0E12,
	MLI_C0E13,
	
	MLI_END__,
} map_list_item;

void map_list_load(map_list_item item);
int map_list_load_custom(char const *path);
void map_list_hard_reload(void);
void map_list_soft_reload(void);
void map_list_load_next(void);

#endif
