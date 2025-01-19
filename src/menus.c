#include "menus.h"

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "conf.h"
#include "editor.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "map_list.h"
#include "options.h"
#include "sound.h"
#include "text.h"
#include "triggers.h"
#include "ui.h"
#include "util.h"
#include "wnd.h"

#define MAX_LEVEL_SEL_PATH_SIZE 512

static void MainDrawBg(void);
static void PauseDrawBg(void);
static void BtnExitMenu(void);
static void BtnExitToDesktop(void);
static void BtnPlayFromBeginning(void);
static void BtnPlayEditCustomLevel(void);
static void BtnPlayCustomLevel(void);
static void BtnEditCustomLevel(void);
static void BtnForceRetry(void);
static void BtnMainMenu(void);
static void BtnOptions(void);
static void BtnDetectKeyLeft(void);
static void BtnDetectKeyRight(void);
static void BtnDetectKeyJump(void);
static void BtnDetectKeyDashDown(void);
static void BtnDetectKeyPowerjump(void);
static void BtnDetectKeyMenu(void);
static void SldrSfxVolume(float vol);
static void BtnExitOptionsMenu(void);
static void BtnReqNext(void);
static void BtnReqRetry(void);

static bool InMenu = false;
static enum MenuRequest Req = MR_NONE;
static char CustomLevelPath[MAX_LEVEL_SEL_PATH_SIZE];

void
MainMenuLoop(void)
{
	struct UiButton BContinue = UiButton_Create(80, 380, "Continue", NULL);
	struct UiButton BPlay = UiButton_Create(80, 420, "Play from beginning", BtnPlayFromBeginning);
	struct UiButton BPlayCustom = UiButton_Create(80, 460, "Play or edit custom level", BtnPlayEditCustomLevel);
	struct UiButton BEditor = UiButton_Create(80, 500, "Options", BtnOptions);
	struct UiButton BExit = UiButton_Create(80, 540, "Exit to desktop", BtnExitToDesktop);
	
	// `InMenu` is irrelevant for the main menu since it is the main launch
	// screen for game functionality, and it doesn't really make sense to
	// "quit" the main menu.
	for (;;)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
 		Input_HandleEvents();
		
		// update main menu.
		{
			UiButton_Update(&BContinue);
			UiButton_Update(&BPlay);
			UiButton_Update(&BPlayCustom);
			UiButton_Update(&BEditor);
			UiButton_Update(&BExit);
			
			Input_PostUpdate();
		}
		
		// draw pause menu.
		{
			MainDrawBg();
			
			// draw UI.
			{
				Text_DrawStr("CUBIC JUMPING", 80, 60);
				
				UiButton_Draw(&BContinue);
				UiButton_Draw(&BPlay);
				UiButton_Draw(&BPlayCustom);
				UiButton_Draw(&BEditor);
				UiButton_Draw(&BExit);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

void
CustomLevelSelectMenuLoop(void)
{
	struct UiTextField TfPath = UiTextField_Create(80, 380, 20, CustomLevelPath, MAX_LEVEL_SEL_PATH_SIZE - 1);
	struct UiButton BPlayLevel = UiButton_Create(80, 420, "Play custom level", BtnPlayCustomLevel);
	struct UiButton BEditLevel = UiButton_Create(80, 460, "Edit custom level", BtnEditCustomLevel);
	struct UiButton BBack = UiButton_Create(80, 500, "Back", BtnExitMenu);
	
	InMenu = true;
	while (InMenu)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		// update level select menu.
		{
			if (Keybd_Pressed(g_Options.KMenu))
				InMenu = false;
			
			UiTextField_Update(&TfPath);
			UiButton_Update(&BPlayLevel);
			UiButton_Update(&BEditLevel);
			UiButton_Update(&BBack);
			
			Input_PostUpdate();
		}
		
		// draw level select menu.
		{
			MainDrawBg();
			
			// draw UI elements.
			{
				Text_DrawStr("Select level", 80, 60);
				
				UiTextField_Draw(&TfPath);
				UiButton_Draw(&BPlayLevel);
				UiButton_Draw(&BEditLevel);
				UiButton_Draw(&BBack);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

enum MenuRequest
LevelEndMenuLoop(void)
{
	struct UiButton BNext = UiButton_Create(80, 380, "Continue", BtnReqNext);
	struct UiButton BRetry = UiButton_Create(80, 420, "Retry level", BtnReqRetry);
	struct UiButton BMainMenu = UiButton_Create(80, 460, "Main menu", BtnMainMenu);
	
	InMenu = true;
	Req = MR_NONE;
	while (InMenu)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
 		Input_HandleEvents();
		
		// update level end menu.
		{
			UiButton_Update(&BNext);
			UiButton_Update(&BRetry);
			UiButton_Update(&BMainMenu);
			
			Input_PostUpdate();
		}
		
		// draw level end menu.
		{
			static uint8_t Cbg[] = CONF_COLOR_LEVEL_END_BG;
			
			SDL_SetRenderDrawColor(g_Rend, Cbg[0], Cbg[1], Cbg[2], 255);
			SDL_RenderClear(g_Rend);
			
			// draw UI elements.
			{
				uint64_t IlTimeS = g_Game.IlTimeMs / 1000;
				uint64_t IlTimeM = IlTimeS / 60;
				uint64_t TotalTimeS = g_Game.TotalTimeMs / 1000;
				uint64_t TotalTimeM = TotalTimeS / 60;
				
				static char TimeBuf[64];
				sprintf(
					TimeBuf,
					"Time: %lu:%02lu.%02lu (%lu:%02lu.%02lu)",
					IlTimeM,
					IlTimeS % 60,
					g_Game.IlTimeMs % 1000 / 10,
					TotalTimeM,
					TotalTimeS % 60,
					g_Game.TotalTimeMs % 1000 / 10
				);
				
				static char DeathsBuf[64];
				sprintf(
					DeathsBuf,
					"Deaths: %u (%u)",
					g_Game.IlDeaths,
					g_Game.TotalDeaths
				);
				
				Text_DrawStr("Level complete", 80, 60);
				
				Text_DrawStr(g_Map.Name, 80, 140);
				Text_DrawStr(TimeBuf, 80, 180);
				Text_DrawStr(DeathsBuf, 80, 220);
				
				UiButton_Draw(&BNext);
				UiButton_Draw(&BRetry);
				UiButton_Draw(&BMainMenu);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
	
	return Req;
}

void
PauseMenuLoop(void)
{
	struct UiButton BResume = UiButton_Create(80, 380, "Resume", BtnExitMenu);
	struct UiButton BRetry = UiButton_Create(80, 420, "Retry level", BtnForceRetry);
	struct UiButton BMainMenu = UiButton_Create(80, 460, "Main menu", BtnMainMenu);
	
	InMenu = true;
	while (InMenu)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		// update pause menu.
		{
			if (Keybd_Pressed(g_Options.KMenu))
				InMenu = false;
			
			UiButton_Update(&BResume);
			UiButton_Update(&BRetry);
			UiButton_Update(&BMainMenu);
			
			Input_PostUpdate();
		}
		
		// draw pause menu.
		{
			PauseDrawBg();
			
			// draw UI.
			{
				Text_DrawStr("Paused", 80, 60);
				
				UiButton_Draw(&BResume);
				UiButton_Draw(&BRetry);
				UiButton_Draw(&BMainMenu);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

void
OptionsMenuLoop(void)
{
	struct UiButton BKLeft = UiButton_Create(80, 140, "[Left]", BtnDetectKeyLeft);
	struct UiButton BKRight = UiButton_Create(80, 180, "[Right]", BtnDetectKeyRight);
	struct UiButton BKJump = UiButton_Create(80, 220, "[Jump]", BtnDetectKeyJump);
	struct UiButton BKDashDown = UiButton_Create(80, 260, "[Dash down]", BtnDetectKeyDashDown);
	struct UiButton BKPowerjump = UiButton_Create(80, 300, "[Powerjump]", BtnDetectKeyPowerjump);
	struct UiButton BKMenu = UiButton_Create(80, 340, "[Menu]", BtnDetectKeyMenu);
	struct UiSlider SSfxVolume = UiSlider_Create(400, 390, 200, 20, g_Options.SfxVolume, SldrSfxVolume);
	struct UiButton BBack = UiButton_Create(80, 420, "Back", BtnExitOptionsMenu);
	
	InMenu = true;
	while (InMenu)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		// update options menu.
		{
			UiButton_Update(&BKLeft);
			UiButton_Update(&BKRight);
			UiButton_Update(&BKJump);
			UiButton_Update(&BKDashDown);
			UiButton_Update(&BKPowerjump);
			UiButton_Update(&BKMenu);
			UiSlider_Update(&SSfxVolume);
			UiButton_Update(&BBack);
			
			Input_PostUpdate();
		}
		
		// draw options menu.
		{
			MainDrawBg();
			
			// draw UI.
			{
				Text_DrawStr("Options", 80, 60);
				
				UiButton_Draw(&BKLeft);
				UiButton_Draw(&BKRight);
				UiButton_Draw(&BKJump);
				UiButton_Draw(&BKDashDown);
				UiButton_Draw(&BKPowerjump);
				UiButton_Draw(&BKMenu);
				UiSlider_Draw(&SSfxVolume);
				UiButton_Draw(&BBack);
				
				Text_DrawStr(SDL_GetKeyName(g_Options.KLeft), 450, 140);
				Text_DrawStr(SDL_GetKeyName(g_Options.KRight), 450, 180);
				Text_DrawStr(SDL_GetKeyName(g_Options.KJump), 450, 220);
				Text_DrawStr(SDL_GetKeyName(g_Options.KDashDown), 450, 260);
				Text_DrawStr(SDL_GetKeyName(g_Options.KPowerjump), 450, 300);
				Text_DrawStr(SDL_GetKeyName(g_Options.KMenu), 450, 340);
				Text_DrawStr("SFX volume", 80, 385);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

SDL_Keycode
KeyDetectMenuLoop(void)
{
	for (;;)
	{
		uint64_t TickBegin = GetUnixTimeMs();
		
		Input_HandleEvents();
		
		// update key detection menu.
		{
			for (SDL_Keycode i = 0; i < 128; ++i)
			{
				if (Keybd_Pressed(i))
					return i;
			}
			
			for (SDL_Keycode i = 128; i < 1024; ++i)
			{
				if (Keybd_Pressed(i))
					return (i - 128) | 1 << 30;
			}
			
			Input_PostUpdate();
		}
		
		// draw key detection menu.
		{
			static uint8_t Cawbg[] = CONF_COLOR_AWAITING_INPUT_BG;
			SDL_SetRenderDrawColor(g_Rend, Cawbg[0], Cawbg[1], Cawbg[2], 255);
			SDL_RenderClear(g_Rend);
			
			Text_DrawStr("Awaiting input...", 200, 280);
			
			SDL_RenderPresent(g_Rend);
		}
		
		uint64_t TickEnd = GetUnixTimeMs();
		int64_t TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

static void
MainDrawBg(void)
{
	// draw basic background squares effect.
	{
		PauseDrawBg();
	}
	
	// draw DVD screensaver.
	{
		static uint8_t Cbgda[] = CONF_COLOR_BG_DVD_A;
		static uint8_t Cbgdb[] = CONF_COLOR_BG_DVD_B;
		
		static int PosX = 0, PosY = 0;
		static int SpeedX = CONF_BG_DVD_SPEED, SpeedY = CONF_BG_DVD_SPEED;
		static float ColLerp = 0.0f, ColSpeed = CONF_BG_DVD_COL_SPEED;
		
		PosX += SpeedX;
		PosY += SpeedY;
		
		if (PosX < 0 || PosX + CONF_BG_DVD_SIZE >= CONF_WND_WIDTH)
			SpeedX *= -1;
		if (PosY < 0 || PosY + CONF_BG_DVD_SIZE >= CONF_WND_HEIGHT)
			SpeedY *= -1;
		
		ColLerp += ColSpeed;
		if (ColLerp < 0.0f || ColLerp > 1.0f)
			ColSpeed *= -1.0f;
		ColLerp = CLAMP(0.0f, ColLerp, 1.0f);
		
		SDL_Rect r =
		{
			.x = PosX,
			.y = PosY,
			.w = CONF_BG_DVD_SIZE,
			.h = CONF_BG_DVD_SIZE,
		};
		
		uint8_t Col[3] =
		{
			Lerp(Cbgda[0], Cbgdb[0], ColLerp),
			Lerp(Cbgda[1], Cbgdb[1], ColLerp),
			Lerp(Cbgda[2], Cbgdb[2], ColLerp),
		};
		
		SDL_SetRenderDrawColor(g_Rend, Col[0], Col[1], Col[2], 255);
		SDL_RenderFillRect(g_Rend, &r);
	}
}

static void
PauseDrawBg(void)
{
	static uint8_t Cbg[] = CONF_COLOR_BG, Cbgs[] = CONF_COLOR_BG_SQUARE;
	static float FirstSquareX = 0.0f;
	static float FirstSquareY = 0.0f;
	
	// update square positions.
	{
		FirstSquareX -= CONF_BG_SQUARE_SPEED_X;
		if (FirstSquareX <= -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP)
			FirstSquareX += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
		
		FirstSquareY -= CONF_BG_SQUARE_SPEED_Y;
		if (FirstSquareY <= -CONF_BG_SQUARE_SIZE - CONF_BG_SQUARE_GAP)
			FirstSquareY += CONF_BG_SQUARE_SIZE + CONF_BG_SQUARE_GAP;
	}
	
	// render background.
	SDL_SetRenderDrawColor(g_Rend, Cbgs[0], Cbgs[1], Cbgs[2], 255);
	SDL_RenderClear(g_Rend);
	
	SDL_SetRenderDrawColor(g_Rend, Cbg[0], Cbg[1], Cbg[2], 255);
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
BtnExitMenu(void)
{
	InMenu = false;
}

static void
BtnExitToDesktop(void)
{
	exit(0);
}

static void
BtnPlayFromBeginning(void)
{
	MapList_Load(MLI_C0E0);
	g_Game.TotalTimeMs = 0;
	g_Game.TotalDeaths = 0;
	g_Game.Running = true;
	Game_Loop();
}

static void
BtnPlayEditCustomLevel(void)
{
	CustomLevelSelectMenuLoop();
}

static void
BtnPlayCustomLevel(void)
{
	if (MapList_LoadCustom(CustomLevelPath))
		return;
	
	g_Game.TotalTimeMs = 0;
	g_Game.TotalDeaths = 0;
	g_Game.Running = true;
	Game_Loop();
	
	free(g_Map.Data);
	g_TriggerCnt = 0;
}

static void
BtnEditCustomLevel(void)
{
	struct stat StatBuf;
	if (stat(CustomLevelPath, &StatBuf))
	{
		// determine map name based on file path.
		char Name[MAX_LEVEL_SEL_PATH_SIZE + 1] = {0};
		{
			size_t Len = strlen(CustomLevelPath);
			
			size_t First = Len;
			while (First > 0 && CustomLevelPath[First - 1] != '/')
				--First;
			
			size_t Last = First;
			while (Last < Len
			       && strncmp(&CustomLevelPath[Last], ".hfm", 4))
			{
				++Last;
			}
			
			strncpy(Name, &CustomLevelPath[First], Last - First + 1);
		}
		
		if (!Name[0])
		{
			LogErr("menus: could not determine name for new map!");
			return;
		}
		
		// try to create map file if doesn't exist.
		if (Map_CreateFile(CustomLevelPath, Name))
			return;
	}
	
	if (Editor_Init(CustomLevelPath))
		return;
	
	Editor_Loop();
	
	free(g_Map.Data);
	g_TriggerCnt = 0;
}

static void
BtnForceRetry(void)
{
	InMenu = false;
	MapList_HardReload();
}

static void
BtnMainMenu(void)
{
	InMenu = false;
	g_Game.Running = false;
}

static void
BtnOptions(void)
{
	OptionsMenuLoop();
}

static void
BtnDetectKeyLeft(void)
{
	g_Options.KLeft = KeyDetectMenuLoop();
}

static void
BtnDetectKeyRight(void)
{
	g_Options.KRight = KeyDetectMenuLoop();
}

static void
BtnDetectKeyJump(void)
{
	g_Options.KJump = KeyDetectMenuLoop();
}

static void
BtnDetectKeyDashDown(void)
{
	g_Options.KDashDown = KeyDetectMenuLoop();
}

static void
BtnDetectKeyPowerjump(void)
{
	g_Options.KPowerjump = KeyDetectMenuLoop();
}

static void
BtnDetectKeyMenu(void)
{
	g_Options.KMenu = KeyDetectMenuLoop();
}

static void
SldrSfxVolume(float Vol)
{
	g_Options.SfxVolume = Vol;
	Sound_SetSfxVolume(Vol);
}

static void
BtnExitOptionsMenu(void)
{
	Options_WriteToFile(CONF_OPTIONS_FILE);
	InMenu = false;
}

static void
BtnReqNext(void)
{
	InMenu = false;
	Req = MR_NEXT;
}

static void
BtnReqRetry(void)
{
	InMenu = false;
	Req = MR_RETRY;
}
