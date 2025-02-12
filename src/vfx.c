#include "vfx.h"

#include <string.h>

#include <SDL2/SDL.h>
#include <unistd.h>

#include "cam.h"
#include "conf.h"
#include "text.h"
#include "textures.h"
#include "wnd.h"

struct Particle g_Particles[VFX_PARTICLES_MAX];
usize g_ParticleCnt = 0;

struct Decal g_Decals[VFX_DECALS_MAX];
usize g_DecalCnt = 0;

// tables.
struct ParticleData Vfx_ParticleData[PT_END__] =
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
		.ColorA = CONF_COLOR_PLAYER_SHARD,
		.ColorB = CONF_COLOR_PLAYER_SHARD
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

u8 Vfx_DecalTextures[DT_END__] =
{
	TI_DECAL_CHAIN_SHORT, // chain short.
	TI_DECAL_CHAIN_MED, // chain med.
	TI_DECAL_CHAIN_LONG, // chain long.
	TI_DECAL_GRASS_SHORT, // grass short.
	TI_DECAL_GRASS_MED, // grass med.
	TI_DECAL_GRASS_LONG, // grass long.
	TI_DECAL_SIGN, // sign.
	TI_DECAL_ARROW_DOWN, // arrow down.
	TI_DECAL_ARROW_LEFT, // arrow left.
	TI_DECAL_ARROW_RIGHT, // arrow right.
	TI_DECAL_ARROW_UP, // arrow up.
	TI_DECAL_BAR_HORIZONTAL, // bar horizontal.
	TI_DECAL_BAR_VERTICAL, // bar vertical.
	TI_DECAL_SKULL, // skull.
	TI_DECAL_PORTAL, // portal.
	TI_DECAL_VINES_SHORT, // vines short.
	TI_DECAL_VINES_MED, // vines med.
	TI_DECAL_VINES_LONG // vines long.
};

void
Vfx_PutParticle(enum ParticleType Type, f32 x, f32 y)
{
	if (g_ParticleCnt >= VFX_PARTICLES_MAX)
		return;
	
	switch (Type)
	{
	case PT_PLAYER_TRACE:
		g_Particles[g_ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.Lifetime = CONF_PLAYER_TRACE_LIFETIME,
			.Type = PT_PLAYER_TRACE
		};
		break;
	case PT_PLAYER_SHARD:
		g_Particles[g_ParticleCnt++] = (struct Particle)
		{
			.PosX = x,
			.PosY = y,
			.VelX = RandFloat(CONF_PLAYER_SHARD_SPEED_RANGE) - CONF_PLAYER_SHARD_SPEED_RANGE / 2.0f,
			.VelY = RandFloat(CONF_PLAYER_SHARD_SPEED_RANGE) - CONF_PLAYER_SHARD_SPEED_RANGE / 2.0f,
			.Lifetime = RandInt(CONF_PLAYER_SHARD_LIFETIME_RANGE),
			.Type = PT_PLAYER_SHARD
		};
		break;
	case PT_AIR_PUFF:
		g_Particles[g_ParticleCnt++] = (struct Particle)
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
		g_Particles[g_ParticleCnt++] = (struct Particle)
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
		g_Particles[g_ParticleCnt++] = (struct Particle)
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
		g_Particles[g_ParticleCnt++] = (struct Particle)
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
	if (g_ParticleCnt >= VFX_PARTICLES_MAX)
		return;
	
	Vfx_PutParticle(Type, x, y);
	
	// override particle color.
	{
		struct Particle *Last = &g_Particles[g_ParticleCnt - 1];
		Last->OverrideColor[0] = Color[0];
		Last->OverrideColor[1] = Color[1];
		Last->OverrideColor[2] = Color[2];
	}
}

void
Vfx_PutDecal(enum DecalType Type, f32 x, f32 y, u8 Layer)
{
	if (g_DecalCnt >= VFX_DECALS_MAX)
		return;
	
	g_Decals[g_DecalCnt++] = (struct Decal)
	{
		.PosX = x,
		.PosY = y,
		.Layer = Layer,
		.Type = Type
	};
}

void
Vfx_RmDecal(usize Idx)
{
	if (Idx >= g_DecalCnt)
		return;
	
	memmove(
		&g_Decals[Idx],
		&g_Decals[Idx + 1],
		sizeof(struct Decal) * (g_DecalCnt - Idx - 1)
	);
	--g_DecalCnt;
}

void
Vfx_Update(void)
{
	// update particles.
	for (usize i = 0; i < g_ParticleCnt; ++i)
	{
		if (g_Particles[i].Lifetime == 0)
		{
			if (i < g_ParticleCnt - 1)
			{
				memmove(
					&g_Particles[i],
					&g_Particles[i + 1],
					(g_ParticleCnt - i - 1) * sizeof(struct Particle)
				);
			}
			
			--g_ParticleCnt;
			--i;
			continue;
		}
		
		--g_Particles[i].Lifetime;
		
		switch (g_Particles[i].Type)
		{
		case PT_LEFT_WALL_PUFF:
		case PT_RIGHT_WALL_PUFF:
		case PT_GROUND_PUFF:
			g_Particles[i].VelY += CONF_GRAVITY;
			break;
		default:
			break;
		}
		
		g_Particles[i].PosX += g_Particles[i].VelX;
		g_Particles[i].PosY += g_Particles[i].VelY;
	}
}

void
Vfx_DrawParticles(void)
{
	for (usize i = 0; i < g_ParticleCnt; ++i)
	{
		// determine particle drawing parameters.
		struct Particle const *p = &g_Particles[i];
		struct ParticleData Data = Vfx_ParticleData[p->Type];
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

void
Vfx_DrawDecals(u8 Layer, bool ShowLayer)
{
	for (usize i = 0; i < g_DecalCnt; ++i)
	{
		struct Decal const *d = &g_Decals[i];
		enum TextureId Tex = Vfx_DecalTextures[d->Type];
		
		if (d->Layer != Layer)
			continue;
		
		i32 ScrX, ScrY;
		GameToScreenCoord(&ScrX, &ScrY, d->PosX, d->PosY);
		
		i32 TexW, TexH;
		Textures_GetScale(Tex, &TexW, &TexH);
		
		// draw decal texture.
		{
			Textures_Draw(
				Tex,
				ScrX,
				ScrY,
				TexW * g_Cam.Zoom * CONF_DRAW_SCALE * VFX_DECAL_SCALE,
				TexH * g_Cam.Zoom * CONF_DRAW_SCALE * VFX_DECAL_SCALE
			);
		}
		
		// draw decal layer.
		if (ShowLayer)
		{
			static char Buf[32];
			sprintf(Buf, "%x", d->Layer);
			
			Text_DrawStr(Buf, ScrX, ScrY);
		}
	}
}
