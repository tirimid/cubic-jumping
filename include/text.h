#ifndef TEXT_H
#define TEXT_H

#define TEXT_FONT_WIDTH 5
#define TEXT_FONT_HEIGHT 6

void text_draw_ch(char ch, int x, int y);
void text_draw_str(char const *s, int x, int y);
void text_draw_str_bounded(char const *s, int px, int py, int sx, int sy);
void text_box_show(char const *text, unsigned ticks);
void text_box_draw(void);
void text_box_update(void);

#endif
