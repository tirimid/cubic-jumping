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

static void DrawBg(void);
static void BtnSkipSequence(void);

static bool InSeq = false;

void
IntroSequence(void)
{
	Sound_PlaySfx(SI_INTRO);
	
	u32 RemTicks = CONF_INTRO_TICKS;
	while (RemTicks > 0)
	{
		u64 TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		// update intro sequence.
		{
			--RemTicks;
			
			Input_PostUpdate();
		}
		
		// draw intro sequence.
		{
			DrawBg();
			Textures_Draw(TI_GAMING_REI_BORDER, 250, 150, 300, 300);
			SDL_RenderPresent(g_Rend);
		}
		
		u64 TickEnd = GetUnixTimeMs();
		i64 TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

void
CreditsSequence(void)
{
	struct UiButton BSkip = UiButton_Create(510, 550, "Skip credits", BtnSkipSequence);
	
	i32 Scroll = CONF_WND_HEIGHT + CONF_CREDITS_DELAY;
	char const *Credits =
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
	u32 CreditsLines = CountLines(Credits);
	
	InSeq = true;
	while (InSeq)
	{
		u64 TickBegin = GetUnixTimeMs();
		
 		Input_HandleEvents();
		
		// update credits sequence.
		{
			Scroll -= CONF_CREDITS_SCROLL;
			if (Scroll < -TEXT_EFF_HEIGHT * CreditsLines - CONF_CREDITS_DELAY)
				InSeq = false;
			
			UiButton_Update(&BSkip);
			Input_PostUpdate();
		}
		
		// draw credits sequence.
		{
			DrawBg();
			Text_DrawStr(Credits, 80, Scroll);
			UiButton_Draw(&BSkip);
			SDL_RenderPresent(g_Rend);
		}
		
		u64 TickEnd = GetUnixTimeMs();
		i64 TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

static void
DrawBg(void)
{
	static u8 Csbg[] = CONF_COLOR_SEQUENCE_BG;
	static u8 Cshl[] = CONF_COLOR_SEQUENCE_HL;
	static i32 Scroll = 0;
	
	// update background.
	{
		Scroll += CONF_SEQUENCE_BG_SCROLL;
		if (Scroll > CONF_SEQUENCE_BG_GAP)
			Scroll -= CONF_SEQUENCE_BG_GAP;
	}
	
	// draw background.
	{
		SDL_SetRenderDrawColor(g_Rend, Csbg[0], Csbg[1], Csbg[2], 255);
		SDL_RenderClear(g_Rend);
		
		i32 DrawLim = MAX(CONF_WND_WIDTH, CONF_WND_HEIGHT) - Scroll;
		SDL_SetRenderDrawColor(g_Rend, Cshl[0], Cshl[1], Cshl[2], 255);
		for (i32 i = -DrawLim; i < DrawLim; i += CONF_SEQUENCE_BG_GAP)
		{
			i32 X0 = i, X1 = i + DrawLim;
			i32 Y0 = 0, Y1 = DrawLim;
			SDL_RenderDrawLine(g_Rend, X0, Y0, X1, Y1);
		}
	}
}

static void
BtnSkipSequence(void)
{
	InSeq = false;
}
