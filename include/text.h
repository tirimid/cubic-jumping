#ifndef TEXT_H
#define TEXT_H

#include "conf.h"

#define TEXT_FONT_WIDTH 5
#define TEXT_FONT_HEIGHT 6
#define TEXT_EFF_WIDTH (CONF_TEXT_SCALE * (TEXT_FONT_WIDTH + 0.5f))
#define TEXT_EFF_HEIGHT (CONF_TEXT_SCALE * (TEXT_FONT_HEIGHT + 0.5f))

void text_draw_ch(char ch, int x, int y);
void text_draw_str(char const *s, int x, int y);
void text_draw_str_bounded(char const *s, int px, int py, int sx, int sy);
void text_box_draw(char const *text);

#endif
