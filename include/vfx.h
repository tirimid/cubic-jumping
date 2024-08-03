#ifndef VFX_H
#define VFX_H

#include <SDL.h>

typedef enum particle_type
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
} particle_type_t;

typedef struct particle
{
	float pos_x, pos_y;
	float vel_x, vel_y;
	unsigned lifetime;
	unsigned char type;
} particle_t;

void vfx_put_particle(particle_type_t type, float x, float y);
void vfx_update(void);
void vfx_draw(SDL_Renderer *rend);

#endif
