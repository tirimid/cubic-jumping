#ifndef SOUND_H
#define SOUND_H

#include "util.h"

enum SfxId
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
	
	SI_END__
};

i32 Sound_Init(void);
void Sound_Quit(void);
void Sound_SetSfxVolume(f32 Vol);
void Sound_PlaySfx(enum SfxId Id);

#endif
