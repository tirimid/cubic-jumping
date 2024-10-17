#ifndef TEXTURES_H
#define TEXTURES_H

typedef enum texture_id
{
	TI_GAMING_REI_BORDER = 0,
	
	TI_END__,
} texture_id;

int textures_init(void);
void textures_quit(void);
void texture_draw(texture_id id, int x, int y, int w, int h);

#endif
