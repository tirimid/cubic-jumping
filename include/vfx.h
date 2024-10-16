#ifndef VFX_H
#define VFX_H

typedef enum particle_type
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
	PT_AIR_PUFF,
} particle_type;

typedef struct particle
{
	float pos_x, pos_y;
	float vel_x, vel_y;
	unsigned lifetime;
	unsigned char type;
} particle;

void vfx_clear_particles(void);
void vfx_put_particle(particle_type type, float x, float y);
void vfx_update(void);
void vfx_draw(void);

#endif
