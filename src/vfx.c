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
			.pos_x = x,
			.pos_y = y,
			.vel_x = 0.0f,
			.vel_y = 0.0f,
			.lifetime = CONF_PLAYER_TRACE_LIFETIME,
			.type = PT_PLAYER_TRACE,
		};
		break;
	case PT_PLAYER_SHARD:
		particles[nparticles++] = (particle_t)
		{
			.pos_x = x,
			.pos_y = y,
			.vel_x = rand_float(CONF_PLAYER_SHARD_SPEED_X_RANGE) - CONF_PLAYER_SHARD_SPEED_X_RANGE / 2.0f,
			.vel_y = -rand_float(CONF_PLAYER_SHARD_SPEED_Y_RANGE),
			.lifetime = rand_int(CONF_PLAYER_SHARD_LIFETIME_RANGE),
			.type = PT_PLAYER_SHARD,
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
			continue;
		}
		
		--particles[i].lifetime;
		
		switch (particles[i].type)
		{
		case PT_PLAYER_TRACE:
			break;
		case PT_PLAYER_SHARD:
			particles[i].vel_y += CONF_GRAVITY;
			break;
		}
		
		particles[i].pos_x += particles[i].vel_x;
		particles[i].pos_y += particles[i].vel_y;
	}
}

void
vfx_draw(SDL_Renderer *rend)
{
	for (size_t i = 0; i < nparticles; ++i)
	{
		// determine particle drawing parameters.
		uint8_t const *ca, *cb;
		float size_a, size_b;
		unsigned max_lifetime;
		switch (particles[i].type)
		{
		case PT_PLAYER_TRACE:
		{
			static uint8_t cpta[] = CONF_COLOR_PLAYER_TRACE_A;
			static uint8_t cptb[] = CONF_COLOR_PLAYER_TRACE_B;
			
			ca = cpta;
			cb = cptb;
			size_a = CONF_PLAYER_TRACE_SIZE_A;
			size_b = CONF_PLAYER_TRACE_SIZE_B;
			max_lifetime = CONF_PLAYER_TRACE_LIFETIME;
			
			break;
		}
		case PT_PLAYER_SHARD:
		{
			static uint8_t cpsa[] = CONF_COLOR_PLAYER_SHARD_A;
			static uint8_t cpsb[] = CONF_COLOR_PLAYER_SHARD_B;
			
			ca = cpsa;
			cb = cpsb;
			size_a = CONF_PLAYER_SHARD_SIZE_A;
			size_b = CONF_PLAYER_SHARD_SIZE_B;
			max_lifetime = CONF_PLAYER_SHARD_LIFETIME_RANGE;
			
			break;
		}
		}
		
		// draw particles.
		{
			float rel_lifetime = (float)(max_lifetime - particles[i].lifetime) / max_lifetime;
			float size = lerp(size_a, size_b, rel_lifetime);
			
			uint8_t r = lerp(ca[0], cb[0], rel_lifetime);
			uint8_t g = lerp(ca[1], cb[1], rel_lifetime);
			uint8_t b = lerp(ca[2], cb[2], rel_lifetime);
			SDL_SetRenderDrawColor(rend, r, g, b, 255);
			
			relative_draw_rect(rend,
			                   particles[i].pos_x - size / 2.0f,
			                   particles[i].pos_y - size / 2.0f,
			                   size,
			                   size);
		}
	}
}
