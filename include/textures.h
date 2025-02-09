#ifndef TEXTURES_H
#define TEXTURES_H

#include "util.h"

enum TextureId
{
	TI_DECAL_CHAIN_LONG = 0,
	TI_DECAL_CHAIN_MED,
	TI_DECAL_CHAIN_SHORT,
	TI_GAMING_REI_BORDER,
	
	TI_END__
};

i32 Textures_Init(void);
void Textures_Quit(void);
void Textures_Draw(enum TextureId Id, i32 x, i32 y, i32 w, i32 h);

#endif
