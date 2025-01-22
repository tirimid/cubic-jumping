#include "vfx.h"

#include <string.h>

#include <SDL2/SDL.h>
#include <unistd.h>

#include "conf.h"
#include "wnd.h"

struct ParticleData
{
	f32 SizeA, SizeB;
	u32 MaxLifetime;
	u8 ColorA[3], ColorB[3];
};

static struct ParticleData ParticleData[PT_END__] =
{
	// player trace.
	{
		.SizeA = CONF_PLAYER_TRACE_SIZE_A,
		.SizeB = CONF_PLAYER_TRACE_SIZE_B,
		.MaxLifetime = CONF_PLAYER_TRACE_LIFETIME,
		.ColorA = CONF_COLOR_PLAYER_TRACE_A,
		.ColorB = CONF_COLOR_PLAYER_TRACE_B
	},
	
	// player shard.
	{
		.SizeA = CONF_PLAYER_SHARD_SIZE_A,
		.SizeB = CONF_PLAYER_SHARD_SIZE_B,
		.MaxLifetime = CONF_PLAYER_SHARD_LIFETIME_RANGE,
		.ColorA = CONF_COLOR_PLAYER_SHARD_A,
		.ColorB = CONF_COLOR_PLAYER_SHARD_B
	},
	
	// air puff.
	{
		.SizeA = CONF_AIR_PUFF_SIZE_A,
		.SizeB = CONF_AIR_PUFF_SIZE_B,
		.MaxLifetime = CONF_AIR_PUFF_LIFETIME_RANGE,
		.ColorA = CONF_COLOR_AIR_PUFF,
		.ColorB = CONF_COLOR_AIR_PUFF
	},
	
	// left wall puff.
	{
		.SizeA = CONF_WALL_PUFF_SIZE_A,
		.SizeB = CONF_WALL_PUFF_SIZE_B,
		.MaxLifetime = CONF_WALL_PUFF_LIFETIME_RANGE
	},
	
	// right wall puff.
	{
		.SizeA = CONF_WALL_PUFF_SIZE_A,
		.SizeB = CONF_WALL_PUFF_SIZE_B,
		.MaxLifetime = CONF_WALL_PUFF_LIFETIME_RANGE
	},
	
	// ground puff.
	{
		.SizeA = CONF_GROUND_PUFF_SIZE_A,
		.SizeB = CONF_GROUND_PUFF_SIZE_B,
		.MaxLifetime = CONF_GROUND_PUFF_LIFETIME_RANGE
	}
};

static u32 ParticleCnt = 0;
static struct Particle Particles[CONF_MAX_PARTICLES];

void
Vfx_ClearParticles(void)
{
	ParticleCnt = 0;
}

void
Vfx_PutParticle(enum ParticleType Type, f32 x, f32 y)
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
	case PT_LEFT_WALL_PUFF:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = RandFloat(CONF_WALL_PUFF_SPEED_X_RANGE),
			.VelY = -RandFloat(CONF_WALL_PUFF_SPEED_Y_RANGE),
			.Lifetime = RandInt(CONF_WALL_PUFF_LIFETIME_RANGE),
			.Type = PT_LEFT_WALL_PUFF
		};
		break;
	case PT_RIGHT_WALL_PUFF:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = -RandFloat(CONF_WALL_PUFF_SPEED_X_RANGE),
			.VelY = -RandFloat(CONF_WALL_PUFF_SPEED_Y_RANGE),
			.Lifetime = RandInt(CONF_WALL_PUFF_LIFETIME_RANGE),
			.Type = PT_RIGHT_WALL_PUFF
		};
		break;
	case PT_GROUND_PUFF:
		Particles[ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = RandFloat(CONF_GROUND_PUFF_SPEED_X_RANGE) - CONF_GROUND_PUFF_SPEED_X_RANGE / 2.0f,
			.VelY = -RandFloat(CONF_GROUND_PUFF_SPEED_Y_RANGE),
			.Lifetime = RandInt(CONF_GROUND_PUFF_LIFETIME_RANGE),
			.Type = PT_GROUND_PUFF
		};
		break;
	default:
		break;
	}
}

void
Vfx_PutOverrideParticle(enum ParticleType Type, f32 x, f32 y, u8 const *Color)
{
	if (ParticleCnt >= CONF_MAX_PARTICLES)
		return;
	
	Vfx_PutParticle(Type, x, y);
	
	// override particle color.
	{
		struct Particle *Last = &Particles[ParticleCnt - 1];
		Last->OverrideColor[0] = Color[0];
		Last->OverrideColor[1] = Color[1];
		Last->OverrideColor[2] = Color[2];
	}
}

void
Vfx_Update(void)
{
	for (i32 i = 0; i < ParticleCnt; ++i)
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
		case PT_LEFT_WALL_PUFF:
		case PT_RIGHT_WALL_PUFF:
		case PT_GROUND_PUFF:
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
	for (u32 i = 0; i < ParticleCnt; ++i)
	{
		// determine particle drawing parameters.
		struct Particle const *p = &Particles[i];
		struct ParticleData Data = ParticleData[p->Type];
		{
			if (p->OverrideColor[0])
				Data.ColorA[0] = Data.ColorB[0] = p->OverrideColor[0];
			if (p->OverrideColor[1])
				Data.ColorA[1] = Data.ColorB[1] = p->OverrideColor[1];
			if (p->OverrideColor[2])
				Data.ColorA[2] = Data.ColorB[2] = p->OverrideColor[2];
		}
		
		// draw particles.
		{
			f32 RelLifetime = (f32)(Data.MaxLifetime - p->Lifetime) / Data.MaxLifetime;
			f32 Size = Lerp(Data.SizeA, Data.SizeB, RelLifetime);
			
			u8 r = Lerp(Data.ColorA[0], Data.ColorB[0], RelLifetime);
			u8 g = Lerp(Data.ColorA[1], Data.ColorB[1], RelLifetime);
			u8 b = Lerp(Data.ColorA[2], Data.ColorB[2], RelLifetime);
			SDL_SetRenderDrawColor(g_Rend, r, g, b, 255);
			
			RelativeDrawRect(
				p->PosX - Size / 2.0f,
				p->PosY - Size / 2.0f,
				Size,
				Size
			);
		}
	}
}
