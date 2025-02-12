PNG_FILES := $(shell find png -name *.png)
PNG_HEADERS := $(patsubst png/%.png,%_png.h,$(PNG_FILES))

all: $(PNG_HEADERS)

%_png.h: png/%.png
	bin2header -o $@ $<
