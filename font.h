#ifndef _FONT_H_
#define _FONT_H_

#include "common.h"
#include <SDL/SDL.h>

void load_font();
void init_font();
void draw_string(const char *string, SDL_Surface *surface, s16 x, s16 y, SDL_Color font_color);
void close_font();

#endif // _FONT_H_
