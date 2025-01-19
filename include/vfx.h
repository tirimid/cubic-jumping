#ifndef VFX_H
#define VFX_H

enum ParticleType
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
	PT_AIR_PUFF
};

struct Particle
{
	float PosX, PosY;
	float VelX, VelY;
	unsigned Lifetime;
	unsigned char Type;
};

void Vfx_ClearParticles(void);
void Vfx_PutParticle(enum ParticleType Type, float x, float y);
void Vfx_Update(void);
void Vfx_Draw(void);

#endif
