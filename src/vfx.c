#include "vfx.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>

#include "conf.h"
#include "util.h"

static size_t nparticles = 0;
static particle_t particles[CONF_MAX_PARTICLES];

void
vfx_put_particle(particle_type_t type, float x, float y)
{
	if (nparticles >= CONF_MAX_PARTICLES)
		return;
	
	switch (type)
	{
	case PT_PLAYER_TRACE:
		particles[nparticles++] = (particle_t)
		{
			.x = x,
			.y = y,
			.lifetime = CONF_PLAYER_TRACE_LIFETIME,
			.type = PT_PLAYER_TRACE,
		};
		break;
	}
}

void
vfx_update(void)
{
	for (ssize_t i = 0; i < nparticles; ++i)
	{
		if (particles[i].lifetime == 0)
		{
			if (i < nparticles - 1)
			{
				memmove(&particles[i],
				        &particles[i + 1],
				        (nparticles - i - 1) * sizeof(particle_t));
			}
			
			--nparticles;
			--i;
		} else
			--particles[i].lifetime;
	}
}

void
vfx_draw(SDL_Renderer *rend)
{
	for (size_t i = 0; i < nparticles; ++i)
	{
		switch (particles[i].type)
		{
		case PT_PLAYER_TRACE:
		{
			static uint8_t cpta[] = CONF_COLOR_PLAYER_TRACE_A;
			static uint8_t cptb[] = CONF_COLOR_PLAYER_TRACE_B;
			
			float lt_frac = (float)(CONF_PLAYER_TRACE_LIFETIME - particles[i].lifetime) / CONF_PLAYER_TRACE_LIFETIME;
			
			float size = lerp(CONF_PLAYER_TRACE_SIZE_A,
			                  CONF_PLAYER_TRACE_SIZE_B,
			                  lt_frac);
			
			uint8_t r = lerp(cpta[0], cptb[0], lt_frac);
			uint8_t g = lerp(cpta[1], cptb[1], lt_frac);
			uint8_t b = lerp(cpta[2], cptb[2], lt_frac);
			
			SDL_SetRenderDrawColor(rend, r, g, b, 255);
			
			relative_draw_rect(rend,
			                   particles[i].x - size / 2.0f,
			                   particles[i].y - size / 2.0f,
			                   size,
			                   size);
			
			break;
		}
		}
	}
}
