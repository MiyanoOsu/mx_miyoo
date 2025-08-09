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

void draw_string(const char *string, SDL_Surface *surface, s16 x, s16 y, SDL_Color font_color) {
    SDL_Rect dst = {x,y,0,0};
    // printf("Font color: R=%d, G=%d, B=%d\n",
    //    font_color.r,
    //    font_color.g,
    //    font_color.b);
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, string, font_color);
    SDL_BlitSurface(text_surface, NULL, surface, &dst);
    SDL_FreeSurface(text_surface);
}

void close_font() {
    TTF_CloseFont(font);
    TTF_Quit();
}