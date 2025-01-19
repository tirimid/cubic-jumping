#ifndef TEXT_H
#define TEXT_H

#include "conf.h"

#define TEXT_FONT_WIDTH 5
#define TEXT_FONT_HEIGHT 6
#define TEXT_EFF_WIDTH (CONF_TEXT_SCALE * (TEXT_FONT_WIDTH + 0.5f))
#define TEXT_EFF_HEIGHT (CONF_TEXT_SCALE * (TEXT_FONT_HEIGHT + 0.5f))

void Text_DrawCh(char Ch, int x, int y);
void Text_DrawStr(char const *s, int x, int y);
void Text_DrawStrBounded(char const *s, int Px, int Py, int Sx, int Sy);
void Text_BoxDraw(char const *Text);

#endif
