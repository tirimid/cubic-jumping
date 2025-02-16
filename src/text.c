#include "text.h"

#include <SDL.h>

#include "wnd.h"

static usize DrawableLenAt(char const *Text, usize Off);

// thanks to azmr on github for creating this header font.
// find the source at `https://github.com/azmr/blit-fonts/blob/master/blit32.h`.
// modified to include the non-printable characters as zeroes.
static u32 Font[] =
{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x08021084, 0x0000294a, 0x15f52bea, 0x08fa38be, 0x33a22e60, 0x2e94d8a6, 0x00001084,
	0x10421088, 0x04421082, 0x00a23880, 0x00471000, 0x04420000, 0x00070000, 0x0c600000, 0x02222200,
	0x1d3ad72e, 0x3e4214c4, 0x3e22222e, 0x1d18320f, 0x210fc888, 0x1d183c3f, 0x1d17844c, 0x0222221f,
	0x1d18ba2e, 0x210f463e, 0x0c6018c0, 0x04401000, 0x10411100, 0x00e03800, 0x04441040, 0x0802322e,
	0x3c1ef62e, 0x231fc544, 0x1f18be2f, 0x3c10862e, 0x1f18c62f, 0x3e10bc3f, 0x0210bc3f, 0x1d1c843e,
	0x2318fe31, 0x3e42109f, 0x0c94211f, 0x23149d31, 0x3e108421, 0x231ad6bb, 0x239cd671, 0x1d18c62e,
	0x0217c62f, 0x30eac62e, 0x2297c62f, 0x1d141a2e, 0x0842109f, 0x1d18c631, 0x08454631, 0x375ad631,
	0x22a21151, 0x08421151, 0x3e22221f, 0x1842108c, 0x20820820, 0x0c421086, 0x00004544, 0xbe000000,
	0x00000082, 0x1c97b000, 0x0e949c21, 0x1c10b800, 0x1c94b908, 0x3c1fc5c0, 0x42211c4c, 0x4e87252e,
	0x12949c21, 0x0c210040, 0x8c421004, 0x12519521, 0x0c210842, 0x235aac00, 0x12949c00, 0x0c949800,
	0x4213a526, 0x7087252e, 0x02149800, 0x0e837000, 0x0c213c42, 0x0e94a400, 0x0464a400, 0x155ac400,
	0x36426c00, 0x4e872529, 0x1e223c00, 0x1843188c, 0x08421084, 0x0c463086, 0x0006d800, 0x00000000
};

void
Text_DrawCh(char Ch, i32 x, i32 y)
{
	static u8 Ct[] = CONF_COLOR_TEXT, Cto[] = CONF_COLOR_TEXT_OUTLINE;
	
	u32 FontChar = Font[(u8)Ch];
	SDL_Renderer *Rend = g_Rend;
	
	// draw outline boxes.
	SDL_SetRenderDrawColor(Rend, Cto[0], Cto[1], Cto[2], 255);
	SDL_Rect r =
	{
		.x = x - CONF_TEXT_OUTLINE_SCALE,
		.y = y - CONF_TEXT_OUTLINE_SCALE,
		.w = CONF_TEXT_SCALE + 2 * CONF_TEXT_OUTLINE_SCALE,
		.h = CONF_TEXT_SCALE + 2 * CONF_TEXT_OUTLINE_SCALE
	};
	
	for (i32 i = 0; i < TEXT_FONT_WIDTH * TEXT_FONT_HEIGHT; ++i)
	{
		if (i > 0 && i % TEXT_FONT_WIDTH == 0)
		{
			r.x = x - CONF_TEXT_OUTLINE_SCALE;
			r.y += CONF_TEXT_SCALE;
		}
		
		if (FontChar & 1 << i)
			SDL_RenderFillRect(Rend, &r);
		
		r.x += CONF_TEXT_SCALE;
	}
	
	// draw inside boxes.
	SDL_SetRenderDrawColor(Rend, Ct[0], Ct[1], Ct[2], 255);
	r = (SDL_Rect)
	{
		.x = x,
		.y = y,
		.w = CONF_TEXT_SCALE,
		.h = CONF_TEXT_SCALE
	};
	
	for (i32 i = 0; i < TEXT_FONT_WIDTH * TEXT_FONT_HEIGHT; ++i)
	{
		if (i > 0 && i % TEXT_FONT_WIDTH == 0)
		{
			r.x = x;
			r.y += CONF_TEXT_SCALE;
		}
		
		if (FontChar & 1 << i)
			SDL_RenderFillRect(Rend, &r);
		
		r.x += CONF_TEXT_SCALE;
	}
}

void
Text_DrawStr(char const *s, i32 x, i32 y)
{
	i32 Dx = 0, Dy = 0;
	for (char const *c = s; *c; ++c)
	{
		if (*c == '\n')
		{
			Dx = 0;
			Dy += TEXT_EFF_HEIGHT;
			continue;
		}
		
		Text_DrawCh(*c, x + Dx, y + Dy);
		Dx += TEXT_EFF_WIDTH;
	}
}

void
Text_DrawStrBounded(char const *s, i32 Px, i32 Py, i32 Sx, i32 Sy)
{
	usize i = 0;
	for (i32 Dy = Py; Dy < Py + Sy; Dy += TEXT_EFF_HEIGHT)
	{
		// skip all non-renderable characters at start of line.
		while (s[i] && !Font[(u8)s[i]])
			++i;
		
		// draw all words on line that don't require a newline wrap.
		for (i32 Dx = Px; Dx < Px + Sx;)
		{
			if (!s[i])
				return;
			
			usize DrawLen = DrawableLenAt(s, i);
			DrawLen = MAX(DrawLen, 1);
			if (Dx + DrawLen * TEXT_EFF_WIDTH >= Px + Sx)
				break;
			
			for (usize j = i; j < i + DrawLen; ++j)
			{
				Text_DrawCh(s[j], Dx, Dy);
				Dx += TEXT_EFF_WIDTH;
			}
			
			i += DrawLen;
		}
	}
}

void
Text_BoxDraw(char const *Text)
{
	// draw actual box of text box.
	{
		static u8 Ctb[] = CONF_COLOR_TEXT_BOX;
		
		SDL_Rect Box =
		{
			.x = 0,
			.y = CONF_WND_HEIGHT - CONF_TEXT_BOX_HEIGHT,
			.w = CONF_WND_WIDTH,
			.h = CONF_TEXT_BOX_HEIGHT,
		};
		
		SDL_SetRenderDrawColor(
			g_Rend,
			Ctb[0],
			Ctb[1],
			Ctb[2],
			CONF_COLOR_TEXT_BOX_OPACITY
		);
		
		SDL_RenderFillRect(g_Rend, &Box);
	}
	
	// draw text.
	{
		Text_DrawStrBounded(
			Text,
			CONF_TEXT_BOX_PADDING,
			CONF_WND_HEIGHT - CONF_TEXT_BOX_HEIGHT + CONF_TEXT_BOX_PADDING,
			CONF_WND_WIDTH - 2 * CONF_TEXT_BOX_PADDING,
			CONF_TEXT_BOX_HEIGHT - 2 * CONF_TEXT_BOX_PADDING
		);
	}
}

static usize
DrawableLenAt(char const *Text, usize Off)
{
	usize Len;
	for (Len = 0; Text[Off + Len]; ++Len)
	{
		if (!Font[(u8)Text[Off + Len]])
			break;
	}
	return Len;
}
