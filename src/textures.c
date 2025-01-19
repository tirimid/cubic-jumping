#include "textures.h"

#include <stddef.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "util.h"
#include "wnd.h"

// compiled texture data.
#include "imgs/gaming_rei_border_png.h"

#define INCLUDE_TEXTURE(Name) \
	{ \
		.Data = Name##_png, \
		.Size = sizeof(Name##_png) \
	}

struct Texture
{
	unsigned char const *Data;
	size_t Size;
	SDL_Texture *Tex;
};

static struct Texture Textures[TI_END__] =
{
	INCLUDE_TEXTURE(gaming_rei_border)
};

int
Textures_Init(void)
{
	atexit(Textures_Quit);
	
	// allocate texture references.
	{
		for (size_t i = 0; i < TI_END__; ++i)
		{
			SDL_RWops *Rwops = SDL_RWFromConstMem(Textures[i].Data, Textures[i].Size);
			if (!Rwops)
			{
				LogErr("textures: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			Textures[i].Tex = IMG_LoadTexture_RW(g_Rend, Rwops, 1);
			if (!Textures[i].Tex)
			{
				LogErr("textures: failed to create SDL_Texture - %s", IMG_GetError());
				return 1;
			}
		}
	}
	
	return 0;
}

void
Textures_Quit(void)
{
	// free texture references.
	{
		for (size_t i = 0; i < TI_END__ && Textures[i].Tex; ++i)
			SDL_DestroyTexture(Textures[i].Tex);
	}
}

void
Textures_Draw(enum TextureId Id, int x, int y, int w, int h)
{
	SDL_Rect r =
	{
		.x = x,
		.y = y,
		.w = w,
		.h = h
	};
	SDL_RenderCopy(g_Rend, Textures[Id].Tex, NULL, &r);
}
