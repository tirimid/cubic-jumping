#include "game.h"

#include <stdlib.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "conf.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "menus.h"
#include "options.h"
#include "player.h"
#include "text.h"
#include "text_list.h"
#include "triggers.h"
#include "util.h"
#include "vfx.h"
#include "wnd.h"

struct Game g_Game;

static void DrawBg(void);
static void FillOutOfBounds(void);
static void DrawIndicators(void);

void
Game_Loop(void)
{
	while (g_Game.Running)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		if (Keybd_Pressed(g_Options.KMenu))
		{
			Input_PostUpdate();
			PauseMenuLoop();
			continue;
		}
		
		// update game.
		{
			Player_Update();
			Triggers_Update();
			Vfx_Update();
			Cam_Update();
			TextList_Update();
			
			Input_PostUpdate();
		}
		
		// draw game.
		{
			DrawBg();
			FillOutOfBounds();
			Map_Draw();
			Vfx_Draw();
			Player_Draw();
#if CONF_SHOW_TRIGGERS
			Triggers_Draw();
#endif
			TextList_Draw();
			DrawIndicators();
			SDL_RenderPresent(g_Rend);
		}
		
		g_Game.IlTimeMs += CONF_TICK_MS;
		g_Game.TotalTimeMs += CONF_TICK_MS;
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

void
Game_DisableSwitches(void)
{
	g_Game.OffSwitches = 0;
	for (size_t i = 0, Size = g_Map.SizeX * g_Map.SizeY; i < Size; ++i)
	{
		if (g_Map.Data[i].Type == MTT_SWITCH_ON)
		{
			g_Map.Data[i].Type = MTT_SWITCH_OFF;
			++g_Game.OffSwitches;
		}
		else if (g_Map.Data[i].Type == MTT_SWITCH_OFF)
			++g_Game.OffSwitches;
	}
	
	if (g_Game.OffSwitches > 0)
	{
		for (size_t i = 0, Size = g_Map.SizeX * g_Map.SizeY; i < Size; ++i)
		{
			if (g_Map.Data[i].Type == MTT_END_ON)
				g_Map.Data[i].Type = MTT_END_OFF;
		}
	}
}

void
Game_EnableSwitch(void)
{
	--g_Game.OffSwitches;
	if (g_Game.OffSwitches == 0)
	{
		for (size_t i = 0, Size = g_Map.SizeX * g_Map.SizeY; i < Size; ++i)
		{
			if (g_Map.Data[i].Type == MTT_END_OFF)
				g_Map.Data[i].Type = MTT_END_ON;
		}
	}
}

static void
DrawBg(void)
{
	static uint8_t Cbg[] = CONF_COLOR_BG, Cbgs[] = CONF_COLOR_BG_SQUARE;
	static float FirstSquareX = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	static float FirstSquareY = -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP;
	
	// update square positions.
	{
		FirstSquareX += CONF_BG_SQUARE_SPEED_X;
		if (FirstSquareX >= 0.0f)
			FirstSquareX -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
		
		FirstSquareY += CONF_BG_SQUARE_SPEED_Y;
		if (FirstSquareY >= 0.0f)
			FirstSquareY -= CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	}
	
	// render background.
	SDL_SetRenderDrawColor(g_Rend, Cbg[0], Cbg[1], Cbg[2], 255);
	SDL_RenderClear(g_Rend);
	
	SDL_SetRenderDrawColor(g_Rend, Cbgs[0], Cbgs[1], Cbgs[2], 255);
	for (float x = FirstSquareX; x < CONF_WND_WIDTH; x += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP)
	{
		for (float y = FirstSquareY; y < CONF_WND_HEIGHT; y += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP)
		{
			SDL_Rect Square =
			{
				.x = x,
				.y = y,
				.w = CONF_BG_SQUARE_SIZE,
				.h = CONF_BG_SQUARE_SIZE,
			};
			SDL_RenderFillRect(g_Rend, &Square);
		}
	}
}

static void
FillOutOfBounds(void)
{
	int ScrLbx, ScrLby;
	GameToScreenCoord(&ScrLbx, &ScrLby, 0.0f, 0.0f);
	
	int ScrUbx, ScrUby;
	GameToScreenCoord(&ScrUbx, &ScrUby, g_Map.SizeX, g_Map.SizeY);
	
	// set OOB cover draw color.
	{
		static uint8_t Cg[] = CONF_COLOR_GROUND;
		SDL_SetRenderDrawColor(g_Rend, Cg[0], Cg[1], Cg[2], 255);
	}
	
	// draw left OOB cover.
	if (ScrLbx > 0)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = ScrLbx,
			.h = CONF_WND_HEIGHT,
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw right OOB cover.
	if (ScrUbx < CONF_WND_WIDTH)
	{
		SDL_Rect r =
		{
			.x = ScrUbx,
			.y = 0,
			.w = CONF_WND_WIDTH - ScrUbx,
			.h = CONF_WND_HEIGHT,
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw top OOB cover.
	if (ScrLby > 0)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = CONF_WND_WIDTH,
			.h = ScrLby,
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw bottom OOB cover.
	if (ScrUby < CONF_WND_HEIGHT)
	{
		SDL_Rect r =
		{
			.x = 0,
			.y = ScrUby,
			.w = CONF_WND_WIDTH,
			.h = CONF_WND_HEIGHT - ScrUby,
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
}

static void
DrawIndicators(void)
{
	// draw IL timer.
	{
		uint64_t IlTimeS = g_Game.IlTimeMs / 1000;
		uint64_t IlTimeM = IlTimeS / 60;
		
		static char Buf[32];
		sprintf(
			Buf,
			"%lu:%02lu.%02lu",
			IlTimeM,
			IlTimeS % 60,
			g_Game.IlTimeMs % 1000 / 10
		);
		
		Text_DrawStr(Buf, 10, 10);
	}
	
	// draw IL death counter.
	{
		static char Buf[32];
		sprintf(
			Buf,
			"%u death%s",
			g_Game.IlDeaths,
			g_Game.IlDeaths == 1 ? "" : "s"
		);
		
		Text_DrawStr(Buf, 10, 50);
	}
}
