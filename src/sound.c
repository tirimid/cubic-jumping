#include "sound.h"

#include <stddef.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "util.h"

// compiled sound data.
#include "sounds/bounce_wav.h"
#include "sounds/dash_down_wav.h"
#include "sounds/death_wav.h"
#include "sounds/end_wav.h"
#include "sounds/intro_wav.h"
#include "sounds/jump_wav.h"
#include "sounds/launch_wav.h"
#include "sounds/powerjump_wav.h"
#include "sounds/switch_wav.h"
#include "sounds/walljump_wav.h"

#define SOUND_FREQ 44100
#define CHUNK_SIZE 2048

#define INCLUDE_SOUND(Name) \
	{ \
		.Data = Name##_wav, \
		.Size = sizeof(Name##_wav) \
	}

struct Sound
{
	unsigned char const *Data;
	size_t Size;
	Mix_Chunk *Chunk;
};

static struct Sound SfxSounds[SI_END__] =
{
	INCLUDE_SOUND(bounce),
	INCLUDE_SOUND(dash_down),
	INCLUDE_SOUND(death),
	INCLUDE_SOUND(end),
	INCLUDE_SOUND(intro),
	INCLUDE_SOUND(jump),
	INCLUDE_SOUND(launch),
	INCLUDE_SOUND(powerjump),
	INCLUDE_SOUND(switch),
	INCLUDE_SOUND(walljump)
};

int
Sound_Init(void)
{
	// initialize SDL mixer resources.
	{
		if (Mix_OpenAudio(SOUND_FREQ, MIX_DEFAULT_FORMAT, 2, CHUNK_SIZE))
		{
			LogErr("sound: failed to open audio device - %s", Mix_GetError());
			return 1;
		}
	}
	
	atexit(Sound_Quit);
	
	// allocate sound effect references.
	{
		for (size_t i = 0; i < SI_END__; ++i)
		{
			SDL_RWops *Rwops = SDL_RWFromConstMem(SfxSounds[i].Data, SfxSounds[i].Size);
			if (!Rwops)
			{
				LogErr("sound: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			SfxSounds[i].Chunk = Mix_LoadWAV_RW(Rwops, 1);
			if (!SfxSounds[i].Chunk)
			{
				LogErr("sound: failed to create Mix_Chunk - %s", Mix_GetError());
				return 1;
			}
		}
	}
	
	return 0;
}

void
Sound_Quit(void)
{
	// free sound effect references.
	{
		for (size_t i = 0; i < SI_END__ && SfxSounds[i].Chunk; ++i)
			Mix_FreeChunk(SfxSounds[i].Chunk);
	}
	
	// deinitialize SDL mixer resources.
	{
		Mix_CloseAudio();
	}
}

void
Sound_SetSfxVolume(float Vol)
{
	Vol = CLAMP(0.0f, Vol, 1.0f);
	Mix_Volume(-1, Vol * MIX_MAX_VOLUME);
}

void
Sound_PlaySfx(enum SfxId Id)
{
	Mix_PlayChannel(-1, SfxSounds[Id].Chunk, 0);
}
