#ifndef TEXTURES_H
#define TEXTURES_H

enum TextureId
{
	TI_GAMING_REI_BORDER = 0,
	
	TI_END__
};

int Textures_Init(void);
void Textures_Quit(void);
void Textures_Draw(enum TextureId Id, int x, int y, int w, int h);

#endif
