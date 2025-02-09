#ifndef VFX_H
#define VFX_H

#include <stdbool.h>

#include "util.h"

#define VFX_PARTICLES_MAX 256
#define VFX_DECALS_MAX 128

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

enum DecalType
{
	DT_CHAIN_LONG = 0,
	DT_CHAIN_MED,
	DT_CHAIN_SHORT,
	
	DT_END__
};

struct Particle
{
	f32 PosX, PosY;
	f32 VelX, VelY;
	u32 Lifetime;
	u8 OverrideColor[3];
	u8 Type;
};

struct ParticleData
{
	f32 SizeA, SizeB;
	u32 MaxLifetime;
	u8 ColorA[3], ColorB[3];
};

struct Decal
{
	f32 PosX, PosY;
	u8 Layer; // in practice, only 0 and 1 used.
	u8 Type;
};

struct DecalData
{
	f32 Width, Height;
	u8 Texture;
};

extern struct Particle g_Particles[VFX_PARTICLES_MAX];
extern usize g_ParticleCnt;

extern struct Decal g_Decals[VFX_DECALS_MAX];
extern usize g_DecalCnt;

// tables.
extern struct ParticleData Vfx_ParticleData[PT_END__];
extern struct DecalData Vfx_DecalData[DT_END__];

void Vfx_PutParticle(enum ParticleType Type, f32 x, f32 y);
void Vfx_PutOverrideParticle(enum ParticleType Type, f32 x, f32 y, u8 const *Color);
void Vfx_PutDecal(enum DecalType Type, f32 x, f32 y, u8 Layer);
void Vfx_RmDecal(usize Idx);
void Vfx_Update(void);
void Vfx_DrawParticles(void);
void Vfx_DrawDecals(u8 Layer, bool ShowLayer);

#endif
