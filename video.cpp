#include "common.h"
#include "menu.h"
#include "config.h"
#include "mx.h"
#include "font.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

SDL_Surface *screen, *layout, *background, *layout_bg, *battery_state[7];

void load_bg() {
    background = IMG_Load(option.bg);
    if(background == NULL) {
        printf("IMG_Load error: %s\n", IMG_GetError());
        exit(-1);
    }
}

const char *battery_image[7] = {
    "battery/0.png",
    "battery/1.png",
    "battery/2.png",
    "battery/3.png",
    "battery/4.png",
    "battery/5.png",
    "battery/ac.png",
};



void set_transparent() {
    SDL_SetAlpha(layout_bg, SDL_SRCALPHA, option.transparent);
}

void load_battery() {
    for(u8 i = 0; i<7; i++) {
        battery_state[i] = IMG_Load(battery_image[i]);
        if(!battery_state[i]) {
            printf("IMG_Load error: %s\n", IMG_GetError());
        }
    }
}

void init_video() {
    if(SDL_Init(SDL_INIT_VIDEO)==-1) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }
    SDL_ShowCursor(SDL_DISABLE);
    screen = SDL_SetVideoMode(320,240,16,SDL_SWSURFACE);
    if(screen == NULL) {
        printf("Could not set video mode: %s\n",SDL_GetError());
        return;
    }
    load_bg();
    layout = SDL_CreateRGBSurface(SDL_SWSURFACE,320,240,16,0,0,0,0);
    if(layout == NULL) {
        printf("Could not create surface: %s\n",SDL_GetError());
        return;
    }

    layout_bg = SDL_CreateRGBSurface(SDL_SWSURFACE,320,240,16,0,0,0,0);
    if(layout_bg == NULL) {
        printf("Could not create surface: %s\n",SDL_GetError());
        return;
    }
    SDL_SetColorKey(layout, SDL_SRCCOLORKEY, SDL_MapRGB(layout->format, 0, 0, 0));
    set_transparent();
}

u8 update_bg = 1;

void update_video() {
    SDL_BlitSurface(background,NULL,screen,NULL);
    SDL_BlitSurface(layout_bg,NULL,screen,NULL);
    draw_menu();
    SDL_BlitSurface(layout,NULL,screen,NULL);
    if(update_bg) {
        SDL_Flip(screen);
        update_bg = 0;
    } else if (dirty_count > 0) {
        SDL_UpdateRects(screen, dirty_count, dirty_rects);
        dirty_count = 0; // reset for the next frame
    }
}

void close_video() {
    SDL_FreeSurface(background);
    SDL_FreeSurface(layout_bg);
    SDL_FreeSurface(layout);
    SDL_FreeSurface(screen);
    SDL_Quit();
}