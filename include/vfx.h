#ifndef VFX_H
#define VFX_H

#include <SDL.h>

typedef enum particle_type
{
	PT_PLAYER_TRACE = 0,
} particle_type_t;

typedef struct particle
{
	float x, y;
	unsigned lifetime;
	unsigned char type;
} particle_t;

void vfx_put_particle(particle_type_t type, float x, float y);
void vfx_update(void);
void vfx_draw(SDL_Renderer *rend);

#endif
