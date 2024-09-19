#ifndef TEXT_H
#define TEXT_H

#include <SDL.h>

#define TEXT_FONT_WIDTH 5
#define TEXT_FONT_HEIGHT 6

void text_draw_ch(SDL_Renderer *rend, char ch, int x, int y);
void text_draw_str(SDL_Renderer *rend, char const *s, int x, int y);
void text_draw_str_bounded(SDL_Renderer *rend, char const *s, int px, int py, int sx, int sy);
void text_box_show(char const *text, unsigned ticks);
void text_box_draw(SDL_Renderer *rend);
void text_box_update(void);

#endif
