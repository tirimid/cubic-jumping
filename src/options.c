#include "options.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#include "conf.h"

#define KEY_BUF_SIZE 64
#define VAL_BUF_SIZE 64
#define SCAN_FMT "%63s = %63[^\r\n]"

struct Options g_Options;

static i32 Opts_GetRaw(FILE *Fp, char const *Key, char Out[]);
static i32 Opts_GetKeycode(FILE *Fp, char const *Key, SDL_Keycode *Out);
static i32 Opts_GetFloat(FILE *Fp, char const *Key, f32 *Out);

void
Options_ReturnToDefault(char const *Path)
{
	g_Options = (struct Options)
	{
		// keybind options.
		.KLeft = CONF_DEFAULT_K_LEFT,
		.KRight = CONF_DEFAULT_K_RIGHT,
		.KJump = CONF_DEFAULT_K_JUMP,
		.KDashDown = CONF_DEFAULT_K_DASH_DOWN,
		.KPowerjump = CONF_DEFAULT_K_POWERJUMP,
		.KMenu = CONF_DEFAULT_K_MENU,
		.KEditorLeft = CONF_DEFAULT_K_EDITOR_LEFT,
		.KEditorRight = CONF_DEFAULT_K_EDITOR_RIGHT,
		.KEditorUp = CONF_DEFAULT_K_EDITOR_UP,
		.KEditorDown = CONF_DEFAULT_K_EDITOR_DOWN,
		
		// sound options.
		.SfxVolume = CONF_DEFAULT_SFX_VOLUME,
		.MusicVolume = CONF_DEFAULT_MUSIC_VOLUME
	};
	
	Options_WriteToFile(Path);
}

i32
Options_ReadFromFile(char const *Path)
{
	FILE *Fp = fopen(Path, "rb");
	if (!Fp)
		return 1;
	
	// read keybind options.
	{
		if (Opts_GetKeycode(Fp, "KLeft", &g_Options.KLeft)
			|| Opts_GetKeycode(Fp, "KRight", &g_Options.KRight)
			|| Opts_GetKeycode(Fp, "KJump", &g_Options.KJump)
			|| Opts_GetKeycode(Fp, "KDashDown", &g_Options.KDashDown)
			|| Opts_GetKeycode(Fp, "KPowerjump", &g_Options.KPowerjump)
			|| Opts_GetKeycode(Fp, "KMenu", &g_Options.KMenu)
			|| Opts_GetKeycode(Fp, "KEditorLeft", &g_Options.KEditorLeft)
			|| Opts_GetKeycode(Fp, "KEditorRight", &g_Options.KEditorRight)
			|| Opts_GetKeycode(Fp, "KEditorUp", &g_Options.KEditorUp)
			|| Opts_GetKeycode(Fp, "KEditorDown", &g_Options.KEditorDown))
		{
			fclose(Fp);
			return 1;
		}
	}
	
	// read sound options.
	{
		if (Opts_GetFloat(Fp, "SfxVolume", &g_Options.SfxVolume)
			|| Opts_GetFloat(Fp, "MusicVolume", &g_Options.MusicVolume))
		{
			fclose(Fp);
			return 1;
		}
		
		if (g_Options.SfxVolume > 1.0f)
		{
			fclose(Fp);
			return 1;
		}
		
		if (g_Options.MusicVolume > 1.0f)
		{
			fclose(Fp);
			return 1;
		}
	}
	
	fclose(Fp);
	return 0;
}

i32
Options_WriteToFile(char const *Path)
{
	FILE *Fp = fopen(Path, "wb");
	if (!Fp)
	{
		LogErr("options: could not open configuration for reading - %s!", Path);
		return 1;
	}
	
	// write keybind options.
	{
		char LeftName[32], RightName[32], JumpName[32], DashDownName[32];
		char PowerjumpName[32];
		char MenuName[32];
		char EditorLeftName[32], EditorRightName[32], EditorUpName[32], EditorDownName[32];
		
		sprintf(LeftName, "%s", SDL_GetKeyName(g_Options.KLeft));
		sprintf(RightName, "%s", SDL_GetKeyName(g_Options.KRight));
		sprintf(JumpName, "%s", SDL_GetKeyName(g_Options.KJump));
		sprintf(DashDownName, "%s", SDL_GetKeyName(g_Options.KDashDown));
		sprintf(PowerjumpName, "%s", SDL_GetKeyName(g_Options.KPowerjump));
		sprintf(MenuName, "%s", SDL_GetKeyName(g_Options.KMenu));
		sprintf(EditorLeftName, "%s", SDL_GetKeyName(g_Options.KEditorLeft));
		sprintf(EditorRightName, "%s", SDL_GetKeyName(g_Options.KEditorRight));
		sprintf(EditorUpName, "%s", SDL_GetKeyName(g_Options.KEditorUp));
		sprintf(EditorDownName, "%s", SDL_GetKeyName(g_Options.KEditorDown));
		
		fprintf(
			Fp,
			"# keybind options.\n"
			"KLeft = %s\n"
			"KRight = %s\n"
			"KJump = %s\n"
			"KDashDown = %s\n"
			"KPowerjump = %s\n"
			"KMenu = %s\n"
			"KEditorLeft = %s\n"
			"KEditorRight = %s\n"
			"KEditorUp = %s\n"
			"KEditorDown = %s\n",
			LeftName,
			RightName,
			JumpName,
			DashDownName,
			PowerjumpName,
			MenuName,
			EditorLeftName,
			EditorRightName,
			EditorUpName,
			EditorDownName
		);
	}
	
	// write audio options.
	{
		fprintf(
			Fp,
			"\n# sound options.\n"
			"SfxVolume = %f\n"
			"MusicVolume = %f\n",
			g_Options.SfxVolume,
			g_Options.MusicVolume
		);
	}
	
	fclose(Fp);
	return 0;
}

static i32
Opts_GetRaw(FILE *Fp, char const *Key, char Out[])
{
	fseek(Fp, 0, SEEK_SET);
	
	for (usize Line = 0; !feof(Fp) && !ferror(Fp); ++Line)
	{
		i32 Ch;
		while ((Ch = fgetc(Fp)) != EOF && isspace(Ch))
			;
		
		if (Ch == '#')
		{
			while ((Ch = fgetc(Fp)) != EOF && Ch != '\n')
				;
		}
		
		if (Ch == '\n' || feof(Fp))
			continue;
		
		fseek(Fp, -1, SEEK_CUR);
		char Buf[KEY_BUF_SIZE] = {0};
		if (fscanf(Fp, SCAN_FMT, Buf, Out) != 2)
		{
			LogErr("options: error on line %zu of configuration!", Line);
			return 1;
		}
		
		if (!strcmp(Out, "NONE"))
			Out[0] = 0;
		
		if (!strcmp(Buf, Key))
			return 0;
	}
	
	LogErr("options: didn't find key %s in configuration!", Key);
	return 1;
}

static i32
Opts_GetKeycode(FILE *Fp, char const *Key, SDL_Keycode *Out)
{
	char Buf[VAL_BUF_SIZE] = {0};
	if (Opts_GetRaw(Fp, Key, Buf))
		return 1;
	
	*Out = SDL_GetKeyFromName(Buf);
	if (*Out == SDLK_UNKNOWN)
	{
		LogErr("options: unknown keycode for key %s - '%s'!", Key, Buf);
		return 1;
	}
	
	return 0;
}

static i32
Opts_GetFloat(FILE *Fp, char const *Key, f32 *Out)
{
	char Buf[VAL_BUF_SIZE] = {0};
	if (Opts_GetRaw(Fp, Key, Buf))
		return 1;
	
	errno = 0;
	*Out = strtof(Buf, NULL);
	if (errno)
	{
		LogErr("options: invalid floating point value for key %s - '%s'!", Key, Buf);
		return 1;
	}
	
	return 0;
}
