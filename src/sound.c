#include "sound.h"

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// compiled WAV sound data.
#include "sounds/bounce_wav.h"
#include "sounds/dash_down_wav.h"
#include "sounds/death_wav.h"
#include "sounds/end_wav.h"
#include "sounds/intro_wav.h"
#include "sounds/jump_wav.h"
#include "sounds/launch_wav.h"
#include "sounds/powerjump_wav.h"
#include "sounds/switch_wav.h"
#include "sounds/textbox_wav.h"
#include "sounds/walljump_wav.h"

// compiled MP3 sound data.
#include "sounds/theme0_mp3.h"

#define SOUND_FREQ 44100
#define CHUNK_SIZE 2048

#define INCLUDE_WAV(Name) \
	{ \
		.Data = Name##_wav, \
		.Size = sizeof(Name##_wav) \
	}

#define INCLUDE_MP3(Name) \
	{ \
		.Data = Name##_mp3, \
		.Size = sizeof(Name##_mp3) \
	}

struct Sound
{
	u8 const *Data;
	usize Size;
	union
	{
		Mix_Chunk *Chunk;
		Mix_Music *Music;
	} MixData;
};

static struct Sound SfxSounds[SI_END__] =
{
	INCLUDE_WAV(bounce),
	INCLUDE_WAV(dash_down),
	INCLUDE_WAV(death),
	INCLUDE_WAV(end),
	INCLUDE_WAV(intro),
	INCLUDE_WAV(jump),
	INCLUDE_WAV(launch),
	INCLUDE_WAV(powerjump),
	INCLUDE_WAV(switch),
	INCLUDE_WAV(textbox),
	INCLUDE_WAV(walljump)
};

static struct Sound MusicSounds[MI_END__] =
{
	INCLUDE_MP3(theme0)
};

i32
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
		for (usize i = 0; i < SI_END__; ++i)
		{
			SDL_RWops *Rwops = SDL_RWFromConstMem(SfxSounds[i].Data, SfxSounds[i].Size);
			if (!Rwops)
			{
				LogErr("sound: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			SfxSounds[i].MixData.Chunk = Mix_LoadWAV_RW(Rwops, 1);
			if (!SfxSounds[i].MixData.Chunk)
			{
				LogErr("sound: failed to create Mix_Chunk - %s", Mix_GetError());
				return 1;
			}
		}
	}
	
	// allocate music references.
	{
		for (usize i = 0; i < MI_END__; ++i)
		{
			SDL_RWops *Rwops = SDL_RWFromConstMem(MusicSounds[i].Data, MusicSounds[i].Size);
			if (!Rwops)
			{
				LogErr("sound: failed to create RWops - %s", SDL_GetError());
				return 1;
			}
			
			MusicSounds[i].MixData.Music = Mix_LoadMUS_RW(Rwops, 1);
			if (!MusicSounds[i].MixData.Music)
			{
				LogErr("sound: failed to create Mix_Music - %s", Mix_GetError());
				return 1;
			}
		}
	}
	
	return 0;
}

void
Sound_Quit(void)
{
	// free music references.
	{
		for (usize i = 0; i < MI_END__ && MusicSounds[i].MixData.Music; ++i)
			Mix_FreeMusic(MusicSounds[i].MixData.Music);
	}
	
	// free sound effect references.
	{
		for (usize i = 0; i < SI_END__ && SfxSounds[i].MixData.Chunk; ++i)
			Mix_FreeChunk(SfxSounds[i].MixData.Chunk);
	}
	
	// deinitialize SDL mixer resources.
	{
		Mix_CloseAudio();
	}
}

void
Sound_SetSfxVolume(f32 Vol)
{
	Vol = CLAMP(0.0f, Vol, 1.0f);
	Mix_Volume(-1, Vol * MIX_MAX_VOLUME);
}

void
Sound_SetMusicVolume(f32 Vol)
{
	Vol = CLAMP(0.0f, Vol, 1.0f);
	Mix_VolumeMusic(Vol * MIX_MAX_VOLUME);
}

void
Sound_PlaySfx(enum SfxId Id)
{
	Mix_PlayChannel(-1, SfxSounds[Id].MixData.Chunk, 0);
}

void
Sound_PlayMusic(enum MusicId Id)
{
	if (Mix_PlayingMusic())
		Mix_HaltMusic();
	Mix_PlayMusic(MusicSounds[Id].MixData.Music, -1);
}
