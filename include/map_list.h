#ifndef MAP_LIST_H
#define MAP_LIST_H

#include "util.h"

enum MapListItem
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
	MLI_C0E14,
	MLI_C0E15,
	MLI_C0E16,
	MLI_C0E17,
	MLI_C0E18,
	MLI_C0E19,
	MLI_C0E20,
	MLI_C0E21,
	MLI_C0E22,
	MLI_C0E23,
	MLI_C0E24,
	
	MLI_END__
};

// tables.
extern u16 MapList_ParTimes[MLI_END__][3]; // par time will be '[0]:[1].[2]'.
extern char const *MapList_Names[MLI_END__];

enum MapListItem MapList_CurrentMap(void);
void MapList_Load(enum MapListItem Item);
i32 MapList_LoadCustom(char const *Path);
void MapList_HardReload(void);
void MapList_SoftReload(void);
void MapList_LoadNext(void);

#endif
