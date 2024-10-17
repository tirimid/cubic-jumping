#ifndef SOUND_H
#define SOUND_H

typedef enum sfx_id
{
	SI_BOUNCE = 0,
	SI_DASH_DOWN,
	SI_DEATH,
	SI_END,
	SI_INTRO,
	SI_JUMP,
	SI_LAUNCH,
	SI_POWERJUMP,
	SI_SWITCH,
	SI_WALLJUMP,
	
	SI_END__,
} sfx_id;

int sound_init(void);
void sound_quit(void);
void sound_set_sfx_volume(float vol);
void sound_play_sfx(sfx_id id);

#endif
