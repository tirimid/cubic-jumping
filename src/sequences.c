#include "sequences.h"

#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "conf.h"
#include "input.h"
#include "sound.h"
#include "text.h"
#include "textures.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

static void draw_bg(void);
static void btn_skip_sequence(void);

static bool in_seq = false;

void
intro_sequence(void)
{
	sound_play_sfx(SI_INTRO);
	
	unsigned rem_ticks = CONF_INTRO_TICKS;
	while (rem_ticks > 0)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
		input_handle_events();
		
		// update intro sequence.
		{
			--rem_ticks;
			
			input_post_update();
		}
		
		// draw intro sequence.
		{
			draw_bg();
			texture_draw(TI_GAMING_REI_BORDER, 250, 150, 300, 300);
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

void
credits_sequence(void)
{
	struct ui_button b_skip = ui_button_create(510, 550, "Skip credits", btn_skip_sequence);
	
	int scroll = CONF_WND_HEIGHT + CONF_CREDITS_DELAY;
	char const *credits =
		"CUBIC JUMPING\n"
		"\n\n\n\n\n"
		"Programming\n"
		"- tirimid\n"
		"\n\n\n\n\n"
		"Level design\n"
		"- tirimid\n"
		"\n\n\n\n\n"
		"Playtesting\n"
		"- Brinpos\n"
		"- Platypus_Eradicator\n"
		"- satsualex\n"
		"- tirimid\n"
		"- tqwqk\n"
		"- UltraVedant\n"
		"- Wercles\n"
		"\n\n\n\n\n"
		"Music and sounds\n"
		"- tirimid\n"
		"\n\n\n\n\n"
		"Special thanks\n"
		"- azmr (header font creator)\n"
		"\n\n\n\n\n"
		"And thank YOU for playing!\n"
		"\n\n\n";
	unsigned credits_lines = count_lines(credits);
	
	in_seq = true;
	while (in_seq)
	{
		uint64_t tick_begin = get_unix_time_ms();
		
 		input_handle_events();
		
		// update credits sequence.
		{
			scroll -= CONF_CREDITS_SCROLL;
			if (scroll < -TEXT_EFF_HEIGHT * credits_lines - CONF_CREDITS_DELAY)
				in_seq = false;
			
			ui_button_update(&b_skip);
			input_post_update();
		}
		
		// draw credits sequence.
		{
			draw_bg();
			text_draw_str(credits, 80, scroll);
			ui_button_draw(&b_skip);
			SDL_RenderPresent(g_rend);
		}
		
		uint64_t tick_end = get_unix_time_ms();
		int64_t tick_time_left = CONF_TICK_MS - tick_end + tick_begin;
		if (tick_time_left > 0)
			SDL_Delay(tick_time_left);
	}
}

static void
draw_bg(void)
{
	static uint8_t csbg[] = CONF_COLOR_SEQUENCE_BG;
	static uint8_t cshl[] = CONF_COLOR_SEQUENCE_HL;
	static int scroll = 0;
	
	// update background.
	{
		scroll += CONF_SEQUENCE_BG_SCROLL;
		if (scroll > CONF_SEQUENCE_BG_GAP)
			scroll -= CONF_SEQUENCE_BG_GAP;
	}
	
	// draw background.
	{
		SDL_SetRenderDrawColor(g_rend, csbg[0], csbg[1], csbg[2], 255);
		SDL_RenderClear(g_rend);
		
		int draw_lim = MAX(CONF_WND_WIDTH, CONF_WND_HEIGHT) - scroll;
		SDL_SetRenderDrawColor(g_rend, cshl[0], cshl[1], cshl[2], 255);
		for (int i = -draw_lim; i < draw_lim; i += CONF_SEQUENCE_BG_GAP)
		{
			int x0 = i, x1 = i + draw_lim;
			int y0 = 0, y1 = draw_lim;
			SDL_RenderDrawLine(g_rend, x0, y0, x1, y1);
		}
	}
}

static void
btn_skip_sequence(void)
{
	in_seq = false;
}
