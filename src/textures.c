#include "textures.h"

#include <stddef.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "util.h"
#include "wnd.h"

// compiled texture data.
#include "imgs/gaming_rei_border_png.h"

#define INCLUDE_TEXTURE(name) \
	{ \
		.data = name##_png, \
		.size = sizeof(name##_png), \
	}

struct texture
{
	unsigned char const *data;
	size_t size;
	SDL_Texture *tex;
};

static struct texture textures[TI_END__] =
{
	INCLUDE_TEXTURE(gaming_rei_border),
};

int
textures_init(void)
{
	atexit(textures_quit);
	
	// allocate texture references.
	{
		for (size_t i = 0; i < TI_END__; ++i)
		{
			SDL_RWops *rwops = SDL_RWFromConstMem(textures[i].data, textures[i].size);
			if (!rwops)
			{
				log_err("textures: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			textures[i].tex = IMG_LoadTexture_RW(g_rend, rwops, 1);
			if (!textures[i].tex)
			{
				log_err("textures: failed to create SDL_Texture - %s", IMG_GetError());
				return 1;
			}
		}
	}
	
	return 0;
}

void
textures_quit(void)
{
	// free texture references.
	{
		for (size_t i = 0; i < TI_END__ && textures[i].tex; ++i)
			SDL_DestroyTexture(textures[i].tex);
	}
}

void
texture_draw(enum texture_id id, int x, int y, int w, int h)
{
	SDL_Rect r =
	{
		.x = x,
		.y = y,
		.w = w,
		.h = h,
	};
	SDL_RenderCopy(g_rend, textures[id].tex, NULL, &r);
}
