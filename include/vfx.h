#ifndef VFX_H
#define VFX_H

#include "util.h"

enum ParticleType
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
	PT_AIR_PUFF
};

struct Particle
{
	f32 PosX, PosY;
	f32 VelX, VelY;
	u32 Lifetime;
	u8 Type;
};

void Vfx_ClearParticles(void);
void Vfx_PutParticle(enum ParticleType Type, f32 x, f32 y);
void Vfx_Update(void);
void Vfx_Draw(void);

#endif
