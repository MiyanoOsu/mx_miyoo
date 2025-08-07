#include "common.h"
#include "font.h"
#include "config.h"

#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>

TTF_Font *font;

void load_font() {
    font = TTF_OpenFont(option.font, 16);
    if (!font) {
        printf( "Font load error: %s\n", TTF_GetError());
        exit(1);
    }
}

void init_font() {
    TTF_Init();
    load_font();
}

void draw_string(const char *string, SDL_Surface *surface, s16 x, s16 y, u8 red, u8 green, u8 blue) {
    SDL_Color text_color = {red, green, blue};
    SDL_Rect dst = {x,y,0,0};
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, string, text_color);
    SDL_BlitSurface(text_surface, NULL, surface, &dst);
    SDL_FreeSurface(text_surface);
}

void close_font() {
    TTF_CloseFont(font);
    TTF_Quit();
}