#include "vfx.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <unistd.h>

#include "conf.h"
#include "util.h"
#include "wnd.h"

static size_t ParticleCnt = 0;
static struct Particle Particles[CONF_MAX_PARTICLES];

void
Vfx_ClearParticles(void)
{
	ParticleCnt = 0;
}

void
Vfx_PutParticle(enum ParticleType Type, float x, float y)
{
	if (ParticleCnt >= CONF_MAX_PARTICLES)
		return;
	
	switch (Type)
	{
	case PT_PLAYER_TRACE:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.Lifetime = CONF_PLAYER_TRACE_LIFETIME,
			.Type = PT_PLAYER_TRACE
		};
		break;
	case PT_PLAYER_SHARD:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = RandFloat(CONF_PLAYER_SHARD_SPEED_X_RANGE) - CONF_PLAYER_SHARD_SPEED_X_RANGE / 2.0f,
			.VelY = -RandFloat(CONF_PLAYER_SHARD_SPEED_Y_RANGE),
			.Lifetime = RandInt(CONF_PLAYER_SHARD_LIFETIME_RANGE),
			.Type = PT_PLAYER_SHARD
		};
		break;
	case PT_AIR_PUFF:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = RandFloat(CONF_AIR_PUFF_SPEED_X_RANGE) - CONF_AIR_PUFF_SPEED_X_RANGE / 2.0f,
			.VelY = -RandFloat(CONF_AIR_PUFF_SPEED_Y_RANGE),
			.Lifetime = RandInt(CONF_AIR_PUFF_LIFETIME_RANGE),
			.Type = PT_AIR_PUFF
		};
		break;
	}
}

void
Vfx_Update(void)
{
	for (ssize_t i = 0; i < ParticleCnt; ++i)
	{
		if (Particles[i].Lifetime == 0)
		{
			if (i < ParticleCnt - 1)
			{
				memmove(
					&Particles[i],
					&Particles[i + 1],
					(ParticleCnt - i - 1) * sizeof(struct Particle)
				);
			}
			
			--ParticleCnt;
			--i;
			continue;
		}
		
		--Particles[i].Lifetime;
		
		switch (Particles[i].Type)
		{
		case PT_PLAYER_SHARD:
			Particles[i].VelY += CONF_GRAVITY;
			break;
		default:
			break;
		}
		
		Particles[i].PosX += Particles[i].VelX;
		Particles[i].PosY += Particles[i].VelY;
	}
}

void
Vfx_Draw(void)
{
	for (size_t i = 0; i < ParticleCnt; ++i)
	{
		// determine particle drawing parameters.
		uint8_t const *Ca = NULL, *Cb = NULL;
		float SizeA = 0.0f, SizeB = 0.0f;
		unsigned MaxLifetime = 0;
		switch (Particles[i].Type)
		{
		case PT_PLAYER_TRACE:
		{
			static uint8_t Cpta[] = CONF_COLOR_PLAYER_TRACE_A;
			static uint8_t Cptb[] = CONF_COLOR_PLAYER_TRACE_B;
			
			Ca = Cpta;
			Cb = Cptb;
			SizeA = CONF_PLAYER_TRACE_SIZE_A;
			SizeB = CONF_PLAYER_TRACE_SIZE_B;
			MaxLifetime = CONF_PLAYER_TRACE_LIFETIME;
			
			break;
		}
		case PT_PLAYER_SHARD:
		{
			static uint8_t Cpsa[] = CONF_COLOR_PLAYER_SHARD_A;
			static uint8_t Cpsb[] = CONF_COLOR_PLAYER_SHARD_B;
			
			Ca = Cpsa;
			Cb = Cpsb;
			SizeA = CONF_PLAYER_SHARD_SIZE_A;
			SizeB = CONF_PLAYER_SHARD_SIZE_B;
			MaxLifetime = CONF_PLAYER_SHARD_LIFETIME_RANGE;
			
			break;
		}
		case PT_AIR_PUFF:
		{
			static uint8_t Cap[] = CONF_COLOR_AIR_PUFF;
			
			Ca = Cap;
			Cb = Cap;
			SizeA = CONF_AIR_PUFF_SIZE_A;
			SizeB = CONF_AIR_PUFF_SIZE_B;
			MaxLifetime = CONF_AIR_PUFF_LIFETIME_RANGE;
		}
		}
		
		// draw particles.
		{
			float RelLifetime = (float)(MaxLifetime - Particles[i].Lifetime) / MaxLifetime;
			float Size = Lerp(SizeA, SizeB, RelLifetime);
			
			uint8_t r = Lerp(Ca[0], Cb[0], RelLifetime);
			uint8_t g = Lerp(Ca[1], Cb[1], RelLifetime);
			uint8_t b = Lerp(Ca[2], Cb[2], RelLifetime);
			SDL_SetRenderDrawColor(g_Rend, r, g, b, 255);
			
			RelativeDrawRect(
				Particles[i].PosX - Size / 2.0f,
				Particles[i].PosY - Size / 2.0f,
				Size,
				Size
			);
		}
	}
}
