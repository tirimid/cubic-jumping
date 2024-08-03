#include "map.h"

#include <stddef.h>
#include <stdio.h>

#include "cam.h"
#include "conf.h"
#include "util.h"

map_t g_map;

static int rd_uint_8(uint8_t *out, FILE *fp);
static int rd_uint_32(uint32_t *out, FILE *fp);
static void wr_uint_8(FILE *fp, uint8_t u8);
static void wr_uint_32(FILE *fp, uint32_t u32);

int
map_load_from_file(char const *file)
{
	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		log_err("map: failed to open file: %s!", file);
		return 1;
	}
	
	if (fgetc(fp) != '/'
	    || fgetc(fp) != '/'
	    || fgetc(fp) != 'C'
	    || fgetc(fp) != 'J')
	{
		log_err("map: file contains invalid header: %s!", file);
		return 1;
	}
	
	uint32_t size_x, size_y;
	uint32_t player_spawn_x, player_spawn_y;
	if (rd_uint_32(&size_x, fp)
	    || rd_uint_32(&size_y, fp)
	    || rd_uint_32(&player_spawn_x, fp)
	    || rd_uint_32(&player_spawn_y, fp))
	{
		return 1;
	}
	
	g_map.size_x = size_x;
	g_map.size_y = size_y;
	g_map.player_spawn_x = player_spawn_x;
	g_map.player_spawn_y = player_spawn_y;
	g_map.data = malloc(sizeof(map_tile_t) * size_x * size_y);
	
	for (size_t i = 0; i < size_x * size_y; ++i)
	{
		uint8_t type;
		if (rd_uint_8(&type, fp))
			return 1;
		
		g_map.data[i] = (map_tile_t)
		{
			.type = type,
		};
	}
	
	return 0;
}

void
map_grow(uint32_t dx, uint32_t dy)
{
	uint32_t old_size_x = g_map.size_x, old_size_y = g_map.size_y;
	g_map.size_x += dx;
	g_map.size_y += dy;
	
	g_map.data = realloc(g_map.data,
	                     sizeof(map_tile_t) * g_map.size_x * g_map.size_y);
	
	// create new air cells (horizontal).
	{
		size_t mv_len = old_size_x * (old_size_y - 1);
		size_t mv_ind = old_size_x;
		while (mv_len > 0)
		{
			memmove(&g_map.data[mv_ind + dx],
			        &g_map.data[mv_ind],
			        sizeof(map_tile_t) * mv_len);
			memset(&g_map.data[mv_ind], 0, sizeof(map_tile_t) * dx);
			mv_len -= old_size_x;
			mv_ind += g_map.size_x;
		}
	}
	
	// create new air cells (vertical).
	{
		memset(&g_map.data[g_map.size_x * old_size_y],
		       0,
		       sizeof(map_tile_t) * g_map.size_x * dy);
	}
}

int
map_write_to_file(char const *file, char const *name)
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
		
		wr_uint_32(fp, g_map.size_x);
		wr_uint_32(fp, g_map.size_y);
		wr_uint_32(fp, g_map.player_spawn_x);
		wr_uint_32(fp, g_map.player_spawn_y);
		
		for (size_t i = 0; i < g_map.size_x * g_map.size_y; ++i)
			wr_uint_8(fp, g_map.data[i].type);
		
		fprintf(fp, "\n");
	}
	
	// write out inclusion target header text.
	{
		fprintf(fp,
		        "#ifndef %s_HFM\n"
		        "#define %s_HFM\n"
		        "#include \"map.h\"\n"
		        "static map_tile_t %s_data[] =\n"
		        "{\n",
		        name,
		        name,
		        name);
		
		for (size_t i = 0; i < g_map.size_x * g_map.size_y; ++i)
		{
			fprintf(fp,
			        "\t{\n"
			        "\t\t.type = %u,\n"
			        "\t},\n",
			        g_map.data[i].type);
		}
		
		fprintf(fp,
		        "};\n"
		        "static map_t %s =\n"
		        "{\n"
		        "\t.size_x = %u,\n"
		        "\t.size_y = %u,\n"
		        "\t.data = %s_data,\n"
		        "\t.player_spawn_x = %u,\n"
		        "\t.player_spawn_y = %u,\n"
		        "};\n"
		        "#endif\n",
		        name,
		        g_map.size_x,
		        g_map.size_y,
		        name,
		        g_map.player_spawn_x,
		        g_map.player_spawn_y);
	}
	
	return 0;
}

float const *
map_tile_color(map_tile_type_t type)
{
	static float colors[MTT_END__][3] =
	{
		{0, 0, 0}, // ignore, air is not drawn.
		CONF_COLOR_GROUND,
		CONF_COLOR_KILL,
		CONF_COLOR_BOUNCE,
	};
	
	return colors[type];
}

bool
map_tile_collision(map_tile_type_t type)
{
	static bool collision[MTT_END__] =
	{
		false,
		true,
		true,
		true,
	};
	
	return collision[type];
}

void
map_draw(SDL_Renderer *rend)
{
	for (uint32_t x = 0; x < g_map.size_x; ++x)
	{
		for (uint32_t y = 0; y < g_map.size_y; ++y)
		{
			map_tile_t *tile = map_get(x, y);
			if (tile->type == MTT_AIR)
				continue;
			
			float const *col = map_tile_color(tile->type);
			SDL_SetRenderDrawColor(rend, col[0], col[1], col[2], 255);
			relative_draw_rect(rend, x, y, 1.0f, 1.0f);
		}
	}
}

map_tile_t *
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
