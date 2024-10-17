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

#define INCLUDE_SOUND(name) \
	{ \
		.data = name##_wav, \
		.size = sizeof(name##_wav), \
	}

typedef struct sound
{
	unsigned char const *data;
	size_t size;
	Mix_Chunk *chunk;
} sound;

static sound sfx_sounds[SI_END__] =
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
	INCLUDE_SOUND(walljump),
};

int
sound_init(void)
{
	// initialize SDL mixer resources.
	{
		if (Mix_OpenAudio(SOUND_FREQ, MIX_DEFAULT_FORMAT, 2, CHUNK_SIZE))
		{
			log_err("sound: failed to open audio device - %s", Mix_GetError());
			return 1;
		}
	}
	
	atexit(sound_quit);
	
	// allocate sound effect references.
	{
		for (size_t i = 0; i < SI_END__; ++i)
		{
			SDL_RWops *rwops = SDL_RWFromConstMem(sfx_sounds[i].data, sfx_sounds[i].size);
			if (!rwops)
			{
				log_err("sound: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			sfx_sounds[i].chunk = Mix_LoadWAV_RW(rwops, 1);
			if (!sfx_sounds[i].chunk)
			{
				log_err("sound: failed to create Mix_Chunk - %s", Mix_GetError());
				return 1;
			}
		}
	}
	
	return 0;
}

void
sound_quit(void)
{
	// free sound effect references.
	{
		for (size_t i = 0; i < SI_END__ && sfx_sounds[i].chunk; ++i)
			Mix_FreeChunk(sfx_sounds[i].chunk);
	}
	
	// deinitialize SDL mixer resources.
	{
		Mix_CloseAudio();
	}
}

void
sound_set_sfx_volume(float vol)
{
	vol = CLAMP(0.0f, vol, 1.0f);
	Mix_Volume(-1, vol * MIX_MAX_VOLUME);
}

void
sound_play_sfx(sfx_id id)
{
	Mix_PlayChannel(-1, sfx_sounds[id].chunk, 0);
}
