#ifndef TEXTURES_H
#define TEXTURES_H

#include "util.h"

enum TextureId
{
	TI_GAMING_REI_BORDER = 0,
	
	TI_END__
};

i32 Textures_Init(void);
void Textures_Quit(void);
void Textures_Draw(enum TextureId Id, i32 x, i32 y, i32 w, i32 h);

#endif
