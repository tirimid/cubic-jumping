#ifndef VFX_H
#define VFX_H

#include "util.h"

enum ParticleType
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
	PT_AIR_PUFF,
	PT_LEFT_WALL_PUFF,
	PT_RIGHT_WALL_PUFF,
	PT_GROUND_PUFF,
	
	PT_END__
};

struct Particle
{
	f32 PosX, PosY;
	f32 VelX, VelY;
	u32 Lifetime;
	u8 OverrideColor[3];
	u8 Type;
};

void Vfx_ClearParticles(void);
void Vfx_PutParticle(enum ParticleType Type, f32 x, f32 y);
void Vfx_PutOverrideParticle(enum ParticleType Type, f32 x, f32 y, u8 const *Color);
void Vfx_Update(void);
void Vfx_Draw(void);

#endif
