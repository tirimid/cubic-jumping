#include "map.h"

#include <stddef.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "cam.h"
#include "conf.h"
#include "triggers.h"
#include "util.h"
#include "wnd.h"

map g_map;

static int rd_uint_8(uint8_t *out, FILE *fp);
static int rd_uint_32(uint32_t *out, FILE *fp);
static void wr_uint_8(FILE *fp, uint8_t u8);
static void wr_uint_32(FILE *fp, uint32_t u32);

int
map_create_file(char const *file, char const *name)
{
	unsigned name_len = strlen(name);
	if (name_len > MAP_MAX_NAME_LEN)
	{
		log_err("map: map name is too long (%u > %u)!", name_len, MAP_MAX_NAME_LEN);
		return 1;
	}
	
	FILE *fp = fopen(file, "wb");
	if (!fp)
	{
		log_err("map: failed to create file: %s!", file);
		return 1;
	}
	
	// write out header for dummy map.
	{
		fprintf(fp, "//CJ");
		
		char name_buf[MAP_MAX_NAME_LEN] = {0};
		strncpy(name_buf, name, MAP_MAX_NAME_LEN);
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
			wr_uint_8(fp, name_buf[i]);
		
		wr_uint_32(fp, 1);
		wr_uint_32(fp, 1);
		wr_uint_32(fp, 0);
		wr_uint_32(fp, 0);
	}
	
	// write out data for dummy map.
	{
		wr_uint_8(fp, MTT_GROUND);
	}
	
	// write out trigger data for dummy map.
	{
		wr_uint_32(fp, 0);
	}
	
	// write out inclusion target header.
	{
		fprintf(fp,
		        "\n#ifndef %s_HFM\n"
		        "#define %s_HFM\n"
		        "#include <stddef.h>\n"
		        "#include \"map.h\"\n"
		        "#include \"triggers.h\"\n"
		        "#define %s_NTRIGGERS 0\n"
		        "static map_tile %s_map_data[] =\n"
		        "{\n"
		        "{1},\n"
		        "};\n"
		        "static map %s_map =\n"
		        "{\n"
		        "\t.size_x = 1,\n"
		        "\t.size_y = 1,\n"
		        "\t.data = %s_data,\n"
		        "\t.player_spawn_x = 0,\n"
		        "\t.player_spawn_y = 0,\n"
		        "\t.name = \"%s\\0\",\n"
		        "};\n"
		        "static trigger %s_triggers[] =\n"
		        "{\n"
		        "{0.0f,0.0f,0.0f,0.0f,0,0,0}\n"
		        "};\n"
		        "#endif\n",
		        name,
		        name,
		        name,
		        name,
		        name,
		        name,
		        name,
		        name);
	}
	
	fclose(fp);
	
	return 0;
}

int
map_load_from_file(char const *file)
{
	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		log_err("map: failed to open file: %s!", file);
		return 1;
	}
	
	// read map header data.
	{
		if (fgetc(fp) != '/'
		    || fgetc(fp) != '/'
		    || fgetc(fp) != 'C'
		    || fgetc(fp) != 'J')
		{
			log_err("map: file contains invalid header: %s!", file);
			return 1;
		}
		
		memset(g_map.name, 0, sizeof(g_map.name));
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
		{
			uint8_t ch;
			if (rd_uint_8(&ch, fp))
				return 1;
			g_map.name[i] = ch;
		}
		
		if (rd_uint_32(&g_map.size_x, fp)
		    || rd_uint_32(&g_map.size_y, fp)
		    || rd_uint_32(&g_map.player_spawn_x, fp)
		    || rd_uint_32(&g_map.player_spawn_y, fp))
		{
			return 1;
		}
	}
	
	// read main map data.
	{
		g_map.data = malloc(sizeof(map_tile) * g_map.size_x * g_map.size_y);
		for (size_t i = 0; i < g_map.size_x * g_map.size_y; ++i)
		{
			uint8_t type;
			if (rd_uint_8(&type, fp))
				return 1;
			
			g_map.data[i] = (map_tile)
			{
				.type = type,
			};
		}
	}
	
	// read trigger data.
	{
		uint32_t ntriggers;
		if (rd_uint_32(&ntriggers, fp))
			return 1;
		
		g_ntriggers = 0;
		for (uint32_t i = 0; i < ntriggers; ++i)
		{
			trigger new_trigger;
			if (rd_uint_32((uint32_t *)&new_trigger.pos_x, fp)
			    || rd_uint_32((uint32_t *)&new_trigger.pos_y, fp)
			    || rd_uint_32((uint32_t *)&new_trigger.size_x, fp)
			    || rd_uint_32((uint32_t *)&new_trigger.size_y, fp)
			    || rd_uint_32((uint32_t *)&new_trigger.arg, fp)
			    || rd_uint_8((uint8_t *)&new_trigger.single_use, fp)
			    || rd_uint_8(&new_trigger.type, fp))
			{
				log_err("map: failed to read one or more triggers from file: %s!", file);
				return 1;
			}
			
			triggers_add_trigger(&new_trigger);
		}
	}
	
	fclose(fp);
	
	return 0;
}

void
map_grow(uint32_t dx, uint32_t dy)
{
	uint32_t old_size_x = g_map.size_x, old_size_y = g_map.size_y;
	g_map.size_x += dx;
	g_map.size_y += dy;
	
	g_map.data = realloc(g_map.data, sizeof(map_tile) * g_map.size_x * g_map.size_y);
	
	// create new air cells (horizontal).
	{
		size_t mv_len = old_size_x * (old_size_y - 1);
		size_t mv_ind = old_size_x;
		while (mv_len > 0)
		{
			memmove(&g_map.data[mv_ind + dx],
			        &g_map.data[mv_ind],
			        sizeof(map_tile) * mv_len);
			memset(&g_map.data[mv_ind], 0, sizeof(map_tile) * dx);
			mv_len -= old_size_x;
			mv_ind += g_map.size_x;
		}
		
		// need to zero new tiles on last row to prevent phantom tiles from
		// randomly appearing upon map grow.
		memset(&g_map.data[mv_ind], 0, sizeof(map_tile) * dx);
	}
	
	// create new air cells (vertical).
	{
		memset(&g_map.data[g_map.size_x * old_size_y],
		       0,
		       sizeof(map_tile) * g_map.size_x * dy);
	}
}

void
map_refit_bounds(void)
{
	// determine minimum bounds of map.
	uint32_t far_x = 0, far_y = 0;
	for (uint32_t x = 0; x < g_map.size_x; ++x)
	{
		for (uint32_t y = 0; y < g_map.size_y; ++y)
		{
			map_tile *tile = map_get(x, y);
			if (tile->type == MTT_AIR)
				continue;
			
			far_y = MAX(far_y, y);
			far_x = MAX(far_x, x);
		}
	}
	
	// if needed, shrink horizontally and move cell memory.
	if (g_map.size_x > far_x + 1)
	{
		size_t dx = g_map.size_x - far_x - 1;
		size_t mv_ind = g_map.size_x;
		size_t mv_len = g_map.size_x * (g_map.size_y - 1);
		
		while (mv_len > 0)
		{
			memmove(&g_map.data[mv_ind - dx],
			        &g_map.data[mv_ind],
			        sizeof(map_tile) * mv_len);
			
			mv_len -= g_map.size_x;
			mv_ind += far_x + 1;
		}
	}
	
	// resize map to minimum possible size.
	{
		g_map.size_x = far_x + 1;
		g_map.size_y = far_y + 1;
	}
}

int
map_write_to_file(char const *file)
{
	FILE *fp = fopen(file, "wb");
	if (!fp)
	{
		log_err("map: failed to open file: %s!", file);
		return 1;
	}
	
	// write out map data header.
	{
		fprintf(fp, "//CJ");
		
		for (size_t i = 0; i < MAP_MAX_NAME_LEN; ++i)
			wr_uint_8(fp, g_map.name[i]);
		
		wr_uint_32(fp, g_map.size_x);
		wr_uint_32(fp, g_map.size_y);
		wr_uint_32(fp, g_map.player_spawn_x);
		wr_uint_32(fp, g_map.player_spawn_y);
		
		for (size_t i = 0; i < g_map.size_x * g_map.size_y; ++i)
			wr_uint_8(fp, g_map.data[i].type);
	}
	
	// write out trigger data.
	{
		wr_uint_32(fp, g_ntriggers);
		
		for (size_t i = 0; i < g_ntriggers; ++i)
		{
			trigger const *trigger = &g_triggers[i];
			
			wr_uint_32(fp, *(uint32_t *)&trigger->pos_x);
			wr_uint_32(fp, *(uint32_t *)&trigger->pos_y);
			wr_uint_32(fp, *(uint32_t *)&trigger->size_x);
			wr_uint_32(fp, *(uint32_t *)&trigger->size_y);
			wr_uint_32(fp, trigger->arg);
			wr_uint_8(fp, trigger->single_use);
			wr_uint_8(fp, trigger->type);
		}
	}
	
	fprintf(fp, "\n");
	
	// write out inclusion target header text.
	{
		fprintf(fp,
		        "#ifndef %s_HFM\n"
		        "#define %s_HFM\n"
		        "#include <stddef.h>\n"
		        "#include \"map.h\"\n"
		        "#include \"triggers.h\"\n"
		        "#define %s_NTRIGGERS %lu\n"
		        "static map_tile %s_map_data[] =\n"
		        "{\n",
		        g_map.name,
		        g_map.name,
		        g_map.name,
		        g_ntriggers,
		        g_map.name);
		
		for (size_t i = 0; i < g_map.size_x * g_map.size_y; ++i)
			fprintf(fp, "{%u},", g_map.data[i].type);
		
		fprintf(fp,
		        "\n};\n"
		        "static map %s_map =\n"
		        "{\n"
		        "\t.size_x = %u,\n"
		        "\t.size_y = %u,\n"
		        "\t.data = %s_map_data,\n"
		        "\t.player_spawn_x = %u,\n"
		        "\t.player_spawn_y = %u,\n"
		        "\t.name = \"%s\\0\",\n"
		        "};\n"
		        "static trigger %s_triggers[] =\n"
		        "{\n",
		        g_map.name,
		        g_map.size_x,
		        g_map.size_y,
		        g_map.name,
		        g_map.player_spawn_x,
		        g_map.player_spawn_y,
		        g_map.name,
		        g_map.name);
		
		for (size_t i = 0; i < g_ntriggers; ++i)
		{
			fprintf(fp,
			        "{%ff,%ff,%ff,%ff,%u,%u,%u},",
			        g_triggers[i].pos_x,
			        g_triggers[i].pos_y,
			        g_triggers[i].size_x,
			        g_triggers[i].size_y,
			        g_triggers[i].arg,
			        g_triggers[i].single_use,
			        g_triggers[i].type);
		}
		
		fprintf(fp,
		        "{0.0f,0.0f,0.0f,0.0f,0,0,0}\n" // dummy trigger.
		        "};\n"
		        "#endif\n",
		        g_map.name);
	}
	
	fclose(fp);
	
	return 0;
}

uint8_t const *
map_tile_color(map_tile_type type)
{
	static uint8_t colors[MTT_END__][3] =
	{
		CONF_COLOR_BG,
		CONF_COLOR_GROUND,
		CONF_COLOR_KILL,
		CONF_COLOR_BOUNCE,
		CONF_COLOR_LAUNCH,
		CONF_COLOR_END_ON,
		CONF_COLOR_SWITCH_OFF,
		CONF_COLOR_SWITCH_ON,
		CONF_COLOR_END_OFF,
	};
	
	return colors[type];
}

bool
map_tile_collision(map_tile_type type)
{
	static bool collision[MTT_END__] =
	{
		false,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
	};
	
	return collision[type];
}

void
map_draw(void)
{
	for (uint32_t x = 0; x < g_map.size_x; ++x)
	{
		for (uint32_t y = 0; y < g_map.size_y; ++y)
		{
			map_tile *tile = map_get(x, y);
			if (tile->type == MTT_AIR)
				continue;
			
			uint8_t const *col = map_tile_color(tile->type);
			SDL_SetRenderDrawColor(g_rend, col[0], col[1], col[2], 255);
			relative_draw_rect(x, y, 1.0f, 1.0f);
		}
	}
}

void
map_draw_outlines(void)
{
	static uint8_t co[] = CONF_COLOR_OUTLINE;
	
	SDL_SetRenderDrawColor(g_rend, co[0], co[1], co[2], 255);
	for (uint32_t x = 0; x < g_map.size_x; ++x)
	{
		for (uint32_t y = 0; y < g_map.size_y; ++y)
			relative_draw_hollow_rect(x, y, 1.0f, 1.0f);
	}
}

map_tile *
map_get(uint32_t x, uint32_t y)
{
	return &g_map.data[g_map.size_x * y + x];
}

static int
rd_uint_8(uint8_t *out, FILE *fp)
{
	int high = fgetc(fp);
	if (high == EOF)
	{
		log_err("map: failed to read U8 higher-half!");
		return 1;
	}
	
	int low = fgetc(fp);
	if (low == EOF)
	{
		log_err("map: failed to read U8 lower-half!");
		return 1;
	}
	
	static uint8_t val_lookup[] =
	{
		['0'] = 0,
		['1'] = 1,
		['2'] = 2,
		['3'] = 3,
		['4'] = 4,
		['5'] = 5,
		['6'] = 6,
		['7'] = 7,
		['8'] = 8,
		['9'] = 9,
		['a'] = 10,
		['b'] = 11,
		['c'] = 12,
		['d'] = 13,
		['e'] = 14,
		['f'] = 15,
	};
	
	*out = (uint8_t)val_lookup[low];
	*out |= (uint8_t)val_lookup[high] << 4;
	return 0;
}

static int
rd_uint_32(uint32_t *out, FILE *fp)
{
	uint8_t b0, b1, b2, b3;
	if (rd_uint_8(&b3, fp)
	    || rd_uint_8(&b2, fp)
	    || rd_uint_8(&b1, fp)
	    || rd_uint_8(&b0, fp))
	{
		return 1;
	}
	
	*out = (uint32_t)b0;
	*out |= (uint32_t)b1 << 8;
	*out |= (uint32_t)b2 << 16;
	*out |= (uint32_t)b3 << 24;
	return 0;
}

static void
wr_uint_8(FILE *fp, uint8_t u8)
{
	static uint8_t hex_lookup[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
	};
	
	fprintf(fp, "%c%c", hex_lookup[u8 >> 4], hex_lookup[u8 & 0xf]);
}

static void
wr_uint_32(FILE *fp, uint32_t u32)
{
	uint8_t b0 = u32 & 0xff;
	uint8_t b1 = (u32 & 0xff00) >> 8;
	uint8_t b2 = (u32 & 0xff0000) >> 16;
	uint8_t b3 = (u32 & 0xff000000) >> 24;
	
	wr_uint_8(fp, b3);
	wr_uint_8(fp, b2);
	wr_uint_8(fp, b1);
	wr_uint_8(fp, b0);
}
