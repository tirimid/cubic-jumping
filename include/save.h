#ifndef SAVE_H
#define SAVE_H

#include "util.h"

#define SAVE_VER_NULL 0
#define SAVE_VER_CURRENT 1

// save data file structure:
//
// 0:1    version
// 1:2    map
// 2:6    total deaths
// 6:14   total time ms

struct SaveData
{
	u64 TotalTimeMs;
	u32 TotalDeaths;
	u8 Ver;
	u8 Map;
};

extern struct SaveData g_SaveData;

i32 Save_ReadFromFile(char const *Path);
i32 Save_WriteToFile(char const *Path);
i32 Save_Validate(void);

#endif
