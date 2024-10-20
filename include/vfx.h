#ifndef VFX_H
#define VFX_H

enum particle_type
{
	PT_PLAYER_TRACE = 0,
	PT_PLAYER_SHARD,
	PT_AIR_PUFF,
};

struct particle
{
	float pos_x, pos_y;
	float vel_x, vel_y;
	unsigned lifetime;
	unsigned char type;
};

void vfx_clear_particles(void);
void vfx_put_particle(enum particle_type type, float x, float y);
void vfx_update(void);
void vfx_draw(void);

#endif
