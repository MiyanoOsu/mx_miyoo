#ifndef _FONT_H_
#define _FONT_H_

#include "common.h"
#include <SDL/SDL.h>

#define MAX_DIRTY 64

extern SDL_Rect dirty_rects[];
extern u8 dirty_count;

int get_text_width(char *text);
void load_font();
void init_font();
void draw_string(const char *string, SDL_Surface *surface, s16 x, s16 y, SDL_Color font_color);
void close_font();

#endif // _FONT_H_
