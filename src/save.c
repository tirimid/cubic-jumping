#include "save.h"

#include <stdio.h>

#include "map_list.h"

struct SaveData g_SaveData;

static i32 RdUint8(u8 *Out, FILE *Fp);
static i32 RdUint32(u32 *Out, FILE *Fp);
static i32 RdUint64(u64 *Out, FILE *Fp);
static void WrUint8(FILE *Fp, u8 b);
static void WrUint32(FILE *Fp, u32 l);
static void WrUint64(FILE *Fp, u64 q);

i32
Save_ReadFromFile(char const *Path)
{
	FILE *Fp = fopen(Path, "rb");
	if (!Fp)
		return 1;
	
	// read in save data.
	{
		if (RdUint8(&g_SaveData.Ver, Fp)
			|| RdUint8(&g_SaveData.Map, Fp)
			|| RdUint32(&g_SaveData.TotalDeaths, Fp)
			|| RdUint64(&g_SaveData.TotalTimeMs, Fp))
		{
			return 1;
		}
	}
	
	fclose(Fp);
	
	return 0;
}

i32
Save_WriteToFile(char const *Path)
{
	FILE *Fp = fopen(Path, "wb");
	if (!Fp)
		return 1;
	
	// write out save data.
	{
		WrUint8(Fp, g_SaveData.Ver);
		WrUint8(Fp, g_SaveData.Map);
		WrUint32(Fp, g_SaveData.TotalDeaths);
		WrUint64(Fp, g_SaveData.TotalTimeMs);
	}
	
	fclose(Fp);
	
	return 0;
}

i32
Save_Validate(void)
{
	if (g_SaveData.Ver != SAVE_VER_CURRENT)
	{
		LogErr("save: savefile version is old or invalid - %d!", g_SaveData.Ver);
		return 1;
	}
	
	if (!g_SaveData.Map || g_SaveData.Map >= MLI_END__)
	{
		LogErr("save: savefile map is invalid - %d!", g_SaveData.Map);
		return 1;
	}
	
	return 0;
}

static i32
RdUint8(u8 *Out, FILE *Fp)
{
	i32 b = fgetc(Fp);
	if (b == EOF)
		return 1;
	
	*Out = b;
	return 0;
}

static i32
RdUint32(u32 *Out, FILE *Fp)
{
	i32 B0 = fgetc(Fp);
	if (B0 == EOF)
		return 1;
	
	i32 B1 = fgetc(Fp);
	if (B1 == EOF)
		return 1;
	
	i32 B2 = fgetc(Fp);
	if (B2 == EOF)
		return 1;
	
	i32 B3 = fgetc(Fp);
	if (B3 == EOF)
		return 1;
	
	*Out = B0;
	*Out |= (u32)B1 << 8;
	*Out |= (u32)B2 << 16;
	*Out |= (u32)B3 << 24;
	return 0;
}

static i32
RdUint64(u64 *Out, FILE *Fp)
{
	i32 B0 = fgetc(Fp);
	if (B0 == EOF)
		return 1;
	
	i32 B1 = fgetc(Fp);
	if (B1 == EOF)
		return 1;
	
	i32 B2 = fgetc(Fp);
	if (B2 == EOF)
		return 1;
	
	i32 B3 = fgetc(Fp);
	if (B3 == EOF)
		return 1;
	
	i32 B4 = fgetc(Fp);
	if (B4 == EOF)
		return 1;
	
	i32 B5 = fgetc(Fp);
	if (B5 == EOF)
		return 1;
	
	i32 B6 = fgetc(Fp);
	if (B6 == EOF)
		return 1;
	
	i32 B7 = fgetc(Fp);
	if (B7 == EOF)
		return 1;
	
	*Out = B0;
	*Out |= (u64)B1 << 8;
	*Out |= (u64)B2 << 16;
	*Out |= (u64)B3 << 24;
	*Out |= (u64)B4 << 32;
	*Out |= (u64)B5 << 40;
	*Out |= (u64)B6 << 48;
	*Out |= (u64)B7 << 56;
	return 0;
}

static void
WrUint8(FILE *Fp, u8 b)
{
	fputc(b, Fp);
}

static void
WrUint32(FILE *Fp, u32 l)
{
	fputc(l & 0xff, Fp);
	fputc(l >> 8 & 0xff, Fp);
	fputc(l >> 16 & 0xff, Fp);
	fputc(l >> 24 & 0xff, Fp);
}

static void
WrUint64(FILE *Fp, u64 q)
{
	fputc(q & 0xff, Fp);
	fputc(q >> 8 & 0xff, Fp);
	fputc(q >> 16 & 0xff, Fp);
	fputc(q >> 24 & 0xff, Fp);
	fputc(q >> 32 & 0xff, Fp);
	fputc(q >> 40 & 0xff, Fp);
	fputc(q >> 48 & 0xff, Fp);
	fputc(q >> 56 & 0xff, Fp);
}
