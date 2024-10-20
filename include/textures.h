#ifndef TEXTURES_H
#define TEXTURES_H

enum texture_id
{
	TI_GAMING_REI_BORDER = 0,
	
	TI_END__,
};

int textures_init(void);
void textures_quit(void);
void texture_draw(enum texture_id id, int x, int y, int w, int h);

#endif
