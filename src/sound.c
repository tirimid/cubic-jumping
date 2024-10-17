#include "sound.h"

#include <stddef.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "util.h"

// compiled sound data.
#include "bounce_wav.h"
#include "dash_down_wav.h"
#include "death_wav.h"
#include "end_wav.h"
#include "jump_wav.h"
#include "launch_wav.h"
#include "powerjump_wav.h"
#include "switch_wav.h"
#include "walljump_wav.h"

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

static sound sounds[SI_END__] =
{
	INCLUDE_SOUND(bounce),
	INCLUDE_SOUND(dash_down),
	INCLUDE_SOUND(death),
	INCLUDE_SOUND(end),
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
			SDL_RWops *rwops = SDL_RWFromConstMem(sounds[i].data, sounds[i].size);
			if (!rwops)
			{
				log_err("sound: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			sounds[i].chunk = Mix_LoadWAV_RW(rwops, 1);
			if (!sounds[i].chunk)
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
		for (size_t i = 0; i < SI_END__ && sounds[i].chunk; ++i)
			Mix_FreeChunk(sounds[i].chunk);
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
	Mix_PlayChannel(-1, sounds[id].chunk, 0);
}
