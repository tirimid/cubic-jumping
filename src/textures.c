#include "textures.h"

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "wnd.h"

// compiled texture data.
#include "imgs/decal_arrow_down_png.h"
#include "imgs/decal_arrow_left_png.h"
#include "imgs/decal_arrow_right_png.h"
#include "imgs/decal_arrow_up_png.h"
#include "imgs/decal_bar_horizontal_png.h"
#include "imgs/decal_bar_vertical_png.h"
#include "imgs/decal_chain_long_png.h"
#include "imgs/decal_chain_med_png.h"
#include "imgs/decal_chain_short_png.h"
#include "imgs/decal_grass_long_png.h"
#include "imgs/decal_grass_med_png.h"
#include "imgs/decal_grass_short_png.h"
#include "imgs/decal_portal_png.h"
#include "imgs/decal_sign_png.h"
#include "imgs/decal_skull_png.h"
#include "imgs/decal_vines_long_png.h"
#include "imgs/decal_vines_med_png.h"
#include "imgs/decal_vines_short_png.h"
#include "imgs/gaming_rei_border_png.h"

#define INCLUDE_PNG(Name) \
	{ \
		.Data = Name##_png, \
		.Size = sizeof(Name##_png) \
	}

struct Texture
{
	u8 const *Data;
	usize Size;
	SDL_Texture *Tex;
};

static struct Texture Textures[TI_END__] =
{
	INCLUDE_PNG(decal_arrow_down),
	INCLUDE_PNG(decal_arrow_left),
	INCLUDE_PNG(decal_arrow_right),
	INCLUDE_PNG(decal_arrow_up),
	INCLUDE_PNG(decal_bar_horizontal),
	INCLUDE_PNG(decal_bar_vertical),
	INCLUDE_PNG(decal_chain_long),
	INCLUDE_PNG(decal_chain_med),
	INCLUDE_PNG(decal_chain_short),
	INCLUDE_PNG(decal_grass_long),
	INCLUDE_PNG(decal_grass_med),
	INCLUDE_PNG(decal_grass_short),
	INCLUDE_PNG(decal_portal),
	INCLUDE_PNG(decal_sign),
	INCLUDE_PNG(decal_skull),
	INCLUDE_PNG(decal_vines_long),
	INCLUDE_PNG(decal_vines_med),
	INCLUDE_PNG(decal_vines_short),
	INCLUDE_PNG(gaming_rei_border)
};

i32
Textures_Init(void)
{
	atexit(Textures_Quit);
	
	// allocate texture references.
	{
		for (usize i = 0; i < TI_END__; ++i)
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
		for (usize i = 0; i < TI_END__ && Textures[i].Tex; ++i)
			SDL_DestroyTexture(Textures[i].Tex);
	}
}

void
Textures_Draw(enum TextureId Id, i32 x, i32 y, i32 w, i32 h)
{
	SDL_Rect r =
	{
		.x = x,
		.y = y,
		.w = w + 1,
		.h = h + 1
	};
	SDL_RenderCopy(g_Rend, Textures[Id].Tex, NULL, &r);
}

void
Textures_GetScale(enum TextureId Id, i32 *OutX, i32 *OutY)
{
	SDL_QueryTexture(Textures[Id].Tex, NULL, NULL, OutX, OutY);
}
