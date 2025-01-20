#include "editor.h"

#include <stdlib.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "conf.h"
#include "input.h"
#include "map.h"
#include "options.h"
#include "text.h"
#include "triggers.h"
#include "ui.h"
#include "wnd.h"

#define NO_DRAG_REGION 0.0f

enum EditMode
{
	EM_TILE_P = 0,
	EM_TILE_F,
	EM_TRIGGER,
	EM_PLAYER
};

static void UpdateEditor(void);
static void DrawBg(void);
static void DrawIndicators(void);
static void BtnModeTileP(void);
static void BtnModeTileF(void);
static void BtnModeTrigger(void);
static void BtnModePlayer(void);
static void BtnTypeNext(void);
static void BtnTypePrev(void);
static void BtnZoomIn(void);
static void BtnZoomOut(void);
static void BtnSave(void);
static void BtnArgAdd(void);
static void BtnArgSub(void);
static void BtnSingle(void);
static void BtnExit(void);

static char const *MapFile;
static enum EditMode Mode = EM_TILE_P;
static i32 Type = 0;
static bool Unsaved = false;
static f32 DragOrigX = NO_DRAG_REGION, DragOrigY = NO_DRAG_REGION;
static u32 Arg = 0;
static bool SingleUse = true;
static bool Running;

i32
Editor_Init(char const *File)
{
	MapFile = File;
	if (Map_LoadFromFile(File))
		return 1;
	
	// init editor state.
	{
		Mode = EM_TILE_P;
		Type = 0;
		Unsaved = false;
		DragOrigX = DragOrigY = NO_DRAG_REGION;
		Arg = 0;
		SingleUse = true;
		Running = true;
	}
	
	// init camera state.
	{
		g_Cam.PosX = 0.0f;
		g_Cam.PosY = 0.0f;
		g_Cam.Zoom = CONF_CAM_MAX_ZOOM;
	}
	
	return 0;
}

void
Editor_Loop(void)
{
	struct UiButton BModeTileP = UiButton_Create(10, 10, "Tile-P", BtnModeTileP);
	struct UiButton BModeTileF = UiButton_Create(160, 10, "Tile-F", BtnModeTileF);
	struct UiButton BModeTrigger = UiButton_Create(310, 10, "Trigger", BtnModeTrigger);
	struct UiButton BModePlayer = UiButton_Create(480, 10, "Player", BtnModePlayer);
	struct UiButton BZoomIn = UiButton_Create(10, 50, "Zoom+", BtnZoomIn);
	struct UiButton BZoomOut = UiButton_Create(135, 50, "Zoom-", BtnZoomOut);
	struct UiButton BSave = UiButton_Create(260, 50, "Save", BtnSave);
	struct UiButton BArgAdd = UiButton_Create(365, 50, "Arg+", BtnArgAdd);
	struct UiButton BArgSub = UiButton_Create(470, 50, "Arg-", BtnArgSub);
	struct UiButton BSingle = UiButton_Create(575, 50, "Single", BtnSingle);
	struct UiButton BTypeNext = UiButton_Create(320, 90, "Type>", BtnTypeNext);
	struct UiButton BTypePrev = UiButton_Create(450, 90, "Type<", BtnTypePrev);
	struct UiButton BExit = UiButton_Create(580, 90, "Exit", BtnExit);
	
	while (Running)
	{
		u64 TickBegin = GetUnixTimeMs();
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				exit(0);
			case SDL_KEYDOWN:
				if (!e.key.repeat)
					Keybd_SetKeyState(&e, true);
				break;
			case SDL_KEYUP:
				if (!e.key.repeat)
					Keybd_SetKeyState(&e, false);
				break;
			case SDL_MOUSEBUTTONUP:
				Mouse_ReleaseButton(&e);
				break;
			case SDL_MOUSEBUTTONDOWN:
				Mouse_PressButton(&e);
				break;
			default:
				break;
			}
		}
		
		// update editor.
		{
			// update UI.
			{
				UiButton_Update(&BModeTileP);
				UiButton_Update(&BModeTileF);
				UiButton_Update(&BModeTrigger);
				UiButton_Update(&BModePlayer);
				UiButton_Update(&BZoomIn);
				UiButton_Update(&BZoomOut);
				UiButton_Update(&BTypeNext);
				UiButton_Update(&BTypePrev);
				UiButton_Update(&BSave);
				UiButton_Update(&BArgAdd);
				UiButton_Update(&BArgSub);
				UiButton_Update(&BSingle);
				UiButton_Update(&BExit);
			}
			
			UpdateEditor();
			Input_PostUpdate();
		}
		
		// draw editor.
		{
			DrawBg();
			Map_Draw();
			Map_DrawOutlines();
			Triggers_Draw();
			DrawIndicators();
			
			// draw UI.
			{
				UiButton_Draw(&BModeTileP);
				UiButton_Draw(&BModeTileF);
				UiButton_Draw(&BModeTrigger);
				UiButton_Draw(&BModePlayer);
				UiButton_Draw(&BZoomIn);
				UiButton_Draw(&BZoomOut);
				UiButton_Draw(&BTypeNext);
				UiButton_Draw(&BTypePrev);
				UiButton_Draw(&BSave);
				UiButton_Draw(&BArgAdd);
				UiButton_Draw(&BArgSub);
				UiButton_Draw(&BSingle);
				UiButton_Draw(&BExit);
			}
			
			SDL_RenderPresent(g_Rend);
		}
		
		u64 TickEnd = GetUnixTimeMs();
		i64 TickTimeLeft = CONF_TICK_MS - TickEnd + TickBegin;
		SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
	}
}

static void
UpdateEditor(void)
{
	// move camera.
	{
		f32 MvHoriz = Keybd_Down(g_Options.KEditorRight) - Keybd_Down(g_Options.KEditorLeft);
		f32 MvVert = Keybd_Down(g_Options.KEditorDown) - Keybd_Down(g_Options.KEditorUp);
		f32 Speed = Keybd_ShiftHeld() ? CONF_EDITOR_CAM_SPEED_FAST : CONF_EDITOR_CAM_SPEED;
		g_Cam.PosX += Speed * MvHoriz;
		g_Cam.PosY += Speed * MvVert;
	}
	
	// mouse interaction based on mode.
	switch (Mode)
	{
	case EM_TILE_P:
	{
		i32 MouseX, MouseY;
		Mouse_Pos(&MouseX, &MouseY);
		
		if (MouseY < CONF_EDITOR_BAR_SIZE)
			break;
		
		if (Mouse_Down(MB_LEFT))
		{
			Unsaved = true;
			
			f32 SelX, SelY;
			ScreenToGameCoord(&SelX, &SelY, MouseX, MouseY);
			SelX = MAX(0.0f, SelX);
			SelY = MAX(0.0f, SelY);
			
			if (SelX >= g_Map.SizeX)
				Map_Grow((i32)SelX - g_Map.SizeX + 1, 0);
			if (SelY >= g_Map.SizeY)
				Map_Grow(0, (i32)SelY - g_Map.SizeY + 1);
			
			Map_Get((i32)SelX, (i32)SelY)->Type = Type;
		}
		
		break;
	}
	case EM_TILE_F:
	{
		i32 MouseX, MouseY;
		Mouse_Pos(&MouseX, &MouseY);
		
		if (MouseY < CONF_EDITOR_BAR_SIZE)
			break;
		
		f32 DragX, DragY;
		ScreenToGameCoord(&DragX, &DragY, MouseX, MouseY);
		
		if (Mouse_Pressed(MB_LEFT))
		{
			DragOrigX = DragX;
			DragOrigY = DragY;
		}
		else if (Mouse_Released(MB_LEFT))
		{
			Unsaved = true;
			
			if (DragX < DragOrigX)
			{
				f32 Tmp = DragX;
				DragX = DragOrigX;
				DragOrigX = Tmp;
			}
			
			if (DragY < DragOrigY)
			{
				f32 Tmp = DragY;
				DragY = DragOrigY;
				DragOrigY = Tmp;
			}
			
			DragX = CLAMP(0.0f, DragX, g_Map.SizeX - 1);
			DragY = CLAMP(0.0f, DragY, g_Map.SizeY - 1);
			DragOrigX = CLAMP(0.0f, DragOrigX, g_Map.SizeX - 1);
			DragOrigY = CLAMP(0.0f, DragOrigY, g_Map.SizeY - 1);
			
			for (i32 x = DragOrigX; x < (i32)DragX + 1; ++x)
			{
				for (i32 y = DragOrigY; y < (i32)DragY + 1; ++y)
					Map_Get(x, y)->Type = Type;
			}
			
			DragOrigX = DragOrigY = NO_DRAG_REGION;
		}
		
		break;
	}
	case EM_TRIGGER:
	{
		i32 MouseX, MouseY;
		Mouse_Pos(&MouseX, &MouseY);
		
		if (MouseY < CONF_EDITOR_BAR_SIZE)
			break;
		
		f32 DragX, DragY;
		ScreenToGameCoord(&DragX, &DragY, MouseX, MouseY);
		
		if (Mouse_Pressed(MB_LEFT))
		{
			DragOrigX = DragX;
			DragOrigY = DragY;
		}
		else if (Mouse_Released(MB_LEFT))
		{
			Unsaved = true;
			
			if (DragX < DragOrigX)
			{
				f32 Tmp = DragX;
				DragX = DragOrigX;
				DragOrigX = Tmp;
			}
			
			if (DragY < DragOrigY)
			{
				f32 Tmp = DragY;
				DragY = DragOrigY;
				DragOrigY = Tmp;
			}
			
			struct Trigger NewTrigger =
			{
				.PosX = DragOrigX,
				.PosY = DragOrigY,
				.SizeX = DragX - DragOrigX,
				.SizeY = DragY - DragOrigY,
				.Arg = Arg,
				.SingleUse = SingleUse,
				.Type = Type,
			};
			Triggers_AddTrigger(&NewTrigger);
			
			DragOrigX = DragOrigY = NO_DRAG_REGION;
		}
		
		if (Mouse_Down(MB_RIGHT))
		{
			for (usize i = 0; i < g_TriggerCnt; ++i)
			{
				struct Trigger const *Trigger = &g_Triggers[i];
				if (DragX >= Trigger->PosX
					&& DragX < Trigger->PosX + Trigger->SizeX
					&& DragY >= Trigger->PosY
					&& DragY < Trigger->PosY + Trigger->SizeY)
				{
					Triggers_RmTrigger(i);
					Unsaved = true;
				}
			}
		}
		
		break;
	}
	case EM_PLAYER:
	{
		i32 MouseX, MouseY;
		Mouse_Pos(&MouseX, &MouseY);
		
		if (MouseY < CONF_EDITOR_BAR_SIZE)
			break;
		
		if (Mouse_Down(MB_LEFT))
		{
			Unsaved = true;
			
			f32 SelX, SelY;
			ScreenToGameCoord(&SelX, &SelY, MouseX, MouseY);
			SelX = MAX(0.0f, SelX);
			SelY = MAX(0.0f, SelY);
			SelX = (i32)SelX;
			SelY = (i32)SelY;
			
			g_Map.PlayerSpawnX = SelX;
			g_Map.PlayerSpawnY = SelY;
		}
		
		break;
	}
	}
}

static void
DrawBg(void)
{
	static u8 Cbg[] = CONF_COLOR_BG;
	SDL_SetRenderDrawColor(g_Rend, Cbg[0], Cbg[1], Cbg[2], 255);
	SDL_RenderClear(g_Rend);
}

static void
DrawIndicators(void)
{
	// draw player spawn position.
	{
		static u8 Cp[] = CONF_COLOR_PLAYER;
		
		SDL_SetRenderDrawColor(g_Rend, Cp[0], Cp[1], Cp[2], 255);
		
		RelativeDrawRect(
			g_Map.PlayerSpawnX,
			g_Map.PlayerSpawnY,
			CONF_PLAYER_SIZE,
			CONF_PLAYER_SIZE
		);
	}
	
	// draw hover / selection boundary.
	{
		static u8 Cb[] = CONF_COLOR_EDITOR_BOUNDARY;
		
		SDL_SetRenderDrawColor(
			g_Rend,
			Cb[0],
			Cb[1],
			Cb[2],
			CONF_COLOR_EDITOR_BOUNDARY_OPACITY
		);
		
		switch (Mode)
		{
		case EM_TILE_P:
		case EM_PLAYER:
		{
			i32 MouseX, MouseY;
			Mouse_Pos(&MouseX, &MouseY);
			
			f32 SelX, SelY;
			ScreenToGameCoord(&SelX, &SelY, MouseX, MouseY);
			SelX = MAX(0.0f, SelX);
			SelY = MAX(0.0f, SelY);
			SelX = (i32)SelX;
			SelY = (i32)SelY;
			
			RelativeDrawRect(SelX, SelY, 1.0f, 1.0f);
			
			break;
		}
		case EM_TILE_F:
		case EM_TRIGGER:
		{
			if (DragOrigX == NO_DRAG_REGION)
				break;
			
			i32 MouseX, MouseY;
			Mouse_Pos(&MouseX, &MouseY);
			
			f32 DragX, DragY;
			ScreenToGameCoord(&DragX, &DragY, MouseX, MouseY);
			
			f32 Lbx = MIN(DragX, DragOrigX);
			f32 Lby = MIN(DragY, DragOrigY);
			f32 Ubx = MAX(DragX, DragOrigX);
			f32 Uby = MAX(DragY, DragOrigY);
			
			RelativeDrawRect(Lbx, Lby, Ubx - Lbx, Uby - Lby);
			
			break;
		}
		}
	}
	
	// draw editor bar.
	{
		static u8 Cbgs[] = CONF_COLOR_BG_SQUARE;
		
		SDL_Rect r =
		{
			.x = 0,
			.y = 0,
			.w = CONF_WND_WIDTH,
			.h = CONF_EDITOR_BAR_SIZE,
		};
		
		SDL_SetRenderDrawColor(g_Rend, Cbgs[0], Cbgs[1], Cbgs[2], 255);
		SDL_RenderFillRect(g_Rend, &r);
	}
	
	// draw current type indicator.
	{
		static u8 Co[] = CONF_COLOR_OUTLINE;
		
		SDL_Rect r =
		{
			.x = CONF_WND_WIDTH - CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.y = 0,
			.w = CONF_EDITOR_TYPE_INDICATOR_SIZE,
			.h = CONF_EDITOR_TYPE_INDICATOR_SIZE,
		};
		
		switch (Mode)
		{
		case EM_TILE_P:
		case EM_TILE_F:
		{
			u8 const *Col = Map_TileColor[Type];
			SDL_SetRenderDrawColor(g_Rend, Col[0], Col[1], Col[2], 255);
			SDL_RenderFillRect(g_Rend, &r);
			break;
		}
		case EM_TRIGGER:
		{
			u8 const *Col = &Trigger_Color[Type][0];
			SDL_SetRenderDrawColor(g_Rend, Col[0], Col[1], Col[2], 255);
			SDL_RenderFillRect(g_Rend, &r);
			break;
		}
		default:
			break;
		}
		
		SDL_SetRenderDrawColor(g_Rend, Co[0], Co[1], Co[2], 255);
		SDL_RenderDrawRect(g_Rend, &r);
	}
	
	// draw arg indicator.
	{
		static char Buf[32];
		snprintf(Buf, sizeof(Buf), "%08x", Arg);
		Text_DrawStr(Buf, 10, 100);
	}
	
	// draw single use indicator.
	{
		Text_DrawStr(SingleUse ? "Sing." : "Mult.", 200, 100);
	}
	
	// draw save status indicator.
	{
		static u8 Ces[] = CONF_COLOR_EDITOR_SAVED;
		static u8 Ceu[] = CONF_COLOR_EDITOR_UNSAVED;
		
		if (Unsaved)
			SDL_SetRenderDrawColor(g_Rend, Ceu[0], Ceu[1], Ceu[2], 255);
		else
			SDL_SetRenderDrawColor(g_Rend, Ces[0], Ces[1], Ces[2], 255);
		
		SDL_Rect r =
		{
			.x = 0,
			.y = CONF_WND_HEIGHT - CONF_EDITOR_SAVE_INDICATOR_SIZE,
			.w = CONF_WND_WIDTH,
			.h = CONF_EDITOR_SAVE_INDICATOR_SIZE,
		};
		SDL_RenderFillRect(g_Rend, &r);
	}
}

static void
BtnModeTileP(void)
{
	if (Mode != EM_TILE_P && Mode != EM_TILE_F)
		Type = 0;
	Mode = EM_TILE_P;
}

static void
BtnModeTileF(void)
{
	if (Mode != EM_TILE_P && Mode != EM_TILE_F)
		Type = 0;
	Mode = EM_TILE_F;
}

static void
BtnModeTrigger(void)
{
	Type = 0;
	Mode = EM_TRIGGER;
}

static void
BtnModePlayer(void)
{
	Mode = EM_PLAYER;
}

static void
BtnTypeNext(void)
{
	switch (Mode)
	{
	case EM_TILE_P:
	case EM_TILE_F:
		Type = Type == MTT_END__ - 1 ? 0 : Type + 1;
		break;
	case EM_TRIGGER:
		Type = Type == TT_END__ - 1 ? 0 : Type + 1;
		break;
	default:
		break;
	}
}

static void
BtnTypePrev(void)
{
	switch (Mode)
	{
	case EM_TILE_P:
	case EM_TILE_F:
		Type = Type == 0 ? MTT_END__ - 1 : Type - 1;
		break;
	case EM_TRIGGER:
		Type = Type == 0 ? TT_END__ - 1 : Type - 1;
		break;
	default:
		break;
	}
}

static void
BtnZoomIn(void)
{
	g_Cam.Zoom += CONF_EDITOR_CAM_ZOOM;
	g_Cam.Zoom = MIN(g_Cam.Zoom, CONF_CAM_MAX_ZOOM);
}

static void
BtnZoomOut(void)
{
	g_Cam.Zoom -= CONF_EDITOR_CAM_ZOOM;
	g_Cam.Zoom = MAX(g_Cam.Zoom, CONF_CAM_MIN_ZOOM);
}

static void
BtnSave(void)
{
	if (!Unsaved)
		return;
	
	Map_RefitBounds();
	Map_WriteToFile(MapFile);
	Unsaved = false;
}

static void
BtnArgAdd(void)
{
	++Arg;
}

static void
BtnArgSub(void)
{
	--Arg;
}

static void
BtnSingle(void)
{
	SingleUse = !SingleUse;
}

static void
BtnExit(void)
{
	Running = false;
}
