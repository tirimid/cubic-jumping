WAV_FILES := $(shell find wav -name *.wav)
WAV_HEADERS := $(patsubst wav/%.wav,%_wav.h,$(WAV_FILES))
MP3_FILES := $(shell find mp3 -name *.mp3)
MP3_HEADERS := $(patsubst mp3/%.mp3,%_mp3.h,$(MP3_FILES))

all: $(WAV_HEADERS) $(MP3_HEADERS)

%_wav.h: wav/%.wav
	bin2header -o $@ $<

%_mp3.h: mp3/%.mp3
	bin2header -o $@ $<
