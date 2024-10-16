#include "text.h"

#include <stddef.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "util.h"
#include "wnd.h"

static size_t drawable_len_at(char const *text, size_t off);

// thanks to azmr on github for creating this header font.
// find the source at `https://github.com/azmr/blit-fonts/blob/master/blit32.h`.
// modified to include the non-printable characters as zeroes.
static uint32_t font[] =
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
	0x36426c00, 0x4e872529, 0x1e223c00, 0x1843188c, 0x08421084, 0x0c463086, 0x0006d800, 0x00000000,
};

void
text_draw_ch(char ch, int x, int y)
{
	static uint8_t ct[] = CONF_COLOR_TEXT, cto[] = CONF_COLOR_TEXT_OUTLINE;
	
	// draw outline boxes.
	SDL_SetRenderDrawColor(g_rend, cto[0], cto[1], cto[2], 255);
	for (int gy = 0; gy < TEXT_FONT_HEIGHT; ++gy)
	{
		for (int gx = 0; gx < TEXT_FONT_WIDTH; ++gx)
		{
			int shift = gy * TEXT_FONT_WIDTH + gx;
			if (!(font[ch] & 1 << shift))
				continue;
			
			SDL_Rect outline =
			{
				.x = x + CONF_TEXT_SCALE * gx - CONF_TEXT_OUTLINE_SCALE,
				.y = y + CONF_TEXT_SCALE * gy - CONF_TEXT_OUTLINE_SCALE,
				.w = CONF_TEXT_SCALE + 2 * CONF_TEXT_OUTLINE_SCALE,
				.h = CONF_TEXT_SCALE + 2 * CONF_TEXT_OUTLINE_SCALE,
			};
			SDL_RenderFillRect(g_rend, &outline);
		}
	}
	
	// draw inside boxes.
	SDL_SetRenderDrawColor(g_rend, ct[0], ct[1], ct[2], 255);
	for (int gy = 0; gy < TEXT_FONT_HEIGHT; ++gy)
	{
		for (int gx = 0; gx < TEXT_FONT_WIDTH; ++gx)
		{
			int shift = gy * TEXT_FONT_WIDTH + gx;
			if (!(font[ch] & 1 << shift))
				continue;
			
			SDL_Rect main =
			{
				.x = x + CONF_TEXT_SCALE * gx,
				.y = y + CONF_TEXT_SCALE * gy,
				.w = CONF_TEXT_SCALE,
				.h = CONF_TEXT_SCALE,
			};
			SDL_RenderFillRect(g_rend, &main);
		}
	}
}

void
text_draw_str(char const *s, int x, int y)
{
	int dx = 0, dy = 0;
	for (char const *c = s; *c; ++c)
	{
		if (*c == '\n')
		{
			dx = 0;
			dy += TEXT_EFF_HEIGHT;
			continue;
		}
		
		text_draw_ch(*c, x + dx, y + dy);
		dx += TEXT_EFF_WIDTH;
	}
}

void
text_draw_str_bounded(char const *s, int px, int py, int sx, int sy)
{
	size_t i = 0;
	for (int dy = py; dy < py + sy; dy += TEXT_EFF_HEIGHT)
	{
		// skip all non-renderable characters at start of line.
		while (s[i] && !font[s[i]])
			++i;
		
		// draw all words on line that don't require a newline wrap.
		for (int dx = px; dx < px + sx;)
		{
			if (!s[i])
				return;
			
			size_t draw_len = drawable_len_at(s, i);
			draw_len = MAX(draw_len, 1);
			if (dx + draw_len * TEXT_EFF_WIDTH >= px + sx)
				break;
			
			for (size_t j = i; j < i + draw_len; ++j)
			{
				text_draw_ch(s[j], dx, dy);
				dx += TEXT_EFF_WIDTH;
			}
			
			i += draw_len;
		}
	}
}

void
text_box_draw(char const *text)
{
	// draw actual box of text box.
	{
		static uint8_t ctb[] = CONF_COLOR_TEXT_BOX;
		
		SDL_Rect box =
		{
			.x = 0,
			.y = CONF_WND_HEIGHT - CONF_TEXT_BOX_HEIGHT,
			.w = CONF_WND_WIDTH,
			.h = CONF_TEXT_BOX_HEIGHT,
		};
		
		SDL_SetRenderDrawColor(g_rend,
		                       ctb[0],
		                       ctb[1],
		                       ctb[2],
		                       CONF_COLOR_TEXT_BOX_OPACITY);
		
		SDL_RenderFillRect(g_rend, &box);
	}
	
	// draw text.
	{
		text_draw_str_bounded(text,
		                      CONF_TEXT_BOX_PADDING,
		                      CONF_WND_HEIGHT - CONF_TEXT_BOX_HEIGHT + CONF_TEXT_BOX_PADDING,
		                      CONF_WND_WIDTH - 2 * CONF_TEXT_BOX_PADDING,
		                      CONF_TEXT_BOX_HEIGHT - 2 * CONF_TEXT_BOX_PADDING);
	}
}

static size_t
drawable_len_at(char const *text, size_t off)
{
	size_t len;
	for (len = 0; text[off + len]; ++len)
	{
		if (!font[text[off + len]])
			break;
	}
	return len;
}
