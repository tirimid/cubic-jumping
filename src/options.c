#include "options.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

#include "conf.h"
#include "util.h"

#define KEY_BUF_SIZE 64
#define VAL_BUF_SIZE 64
#define SCAN_FMT "%63s = %63[^\r\n]"

options g_options;

static int opts_get_raw(FILE *fp, char const *key, char out[]);
static int opts_get_keycode(FILE *fp, char const *key, SDL_Keycode *out);

void
options_return_to_default(char const *path)
{
	g_options = (options)
	{
		// keybind options.
		.k_left = CONF_DEFAULT_K_LEFT,
		.k_right = CONF_DEFAULT_K_RIGHT,
		.k_jump = CONF_DEFAULT_K_JUMP,
		.k_dash_down = CONF_DEFAULT_K_DASH_DOWN,
		.k_powerjump = CONF_DEFAULT_K_POWERJUMP,
		.k_menu = CONF_DEFAULT_K_MENU,
		.k_editor_left = CONF_DEFAULT_K_EDITOR_LEFT,
		.k_editor_right = CONF_DEFAULT_K_EDITOR_RIGHT,
		.k_editor_up = CONF_DEFAULT_K_EDITOR_UP,
		.k_editor_down = CONF_DEFAULT_K_EDITOR_DOWN,
	};
	
	options_write_to_file(path);
}

int
options_read_from_file(char const *path)
{
	FILE *fp = fopen(path, "rb");
	if (!fp)
	{
		log_err("options: could not read configuration - %s!", path);
		return 1;
	}
	
	// read keybind options.
	{
		if (opts_get_keycode(fp, "k_left", &g_options.k_left)
		    || opts_get_keycode(fp, "k_right", &g_options.k_right)
		    || opts_get_keycode(fp, "k_jump", &g_options.k_jump)
		    || opts_get_keycode(fp, "k_dash_down", &g_options.k_dash_down)
		    || opts_get_keycode(fp, "k_powerjump", &g_options.k_powerjump)
		    || opts_get_keycode(fp, "k_menu", &g_options.k_menu)
		    || opts_get_keycode(fp, "k_editor_left", &g_options.k_editor_left)
		    || opts_get_keycode(fp, "k_editor_right", &g_options.k_editor_right)
		    || opts_get_keycode(fp, "k_editor_up", &g_options.k_editor_up)
		    || opts_get_keycode(fp, "k_editor_down", &g_options.k_editor_down))
		{
			return 1;
		}
	}
	
	return 0;
}

int
options_write_to_file(char const *path)
{
	// TODO: implement.
}

static int
opts_get_raw(FILE *fp, char const *key, char out[])
{
	fseek(fp, 0, SEEK_SET);
	
	for (size_t line = 0; !feof(fp) && !ferror(fp); ++line)
	{
		int ch;
		while ((ch = fgetc(fp)) != EOF && isspace(ch))
			;
		
		if (ch == '#')
		{
			while ((ch = fgetc(fp)) != EOF && ch != '\n')
				;
		}
		
		if (ch == '\n' || feof(fp))
			continue;
		
		fseek(fp, -1, SEEK_CUR);
		char buf[KEY_BUF_SIZE] = {0};
		if (fscanf(fp, SCAN_FMT, buf, out) != 2)
		{
			log_err("options: error on line %zu of configuration!", line);
			return 1;
		}
		
		if (!strcmp(out, "NONE"))
			out[0] = 0;
		
		if (!strcmp(buf, key))
			return 0;
	}
	
	log_err("options: didn't find key %s in configuration!", key);
	return 1;
}

static int
opts_get_keycode(FILE *fp, char const *key, SDL_Keycode *out)
{
	char buf[VAL_BUF_SIZE] = {0};
	if (opts_get_raw(fp, key, buf))
		return 1;
	
	*out = SDL_GetKeyFromName(buf);
	if (*out == SDLK_UNKNOWN)
	{
		log_err("options: unknown keycode for key %s - '%s'!", key, buf);
		return 1;
	}
	
	return 0;
}
