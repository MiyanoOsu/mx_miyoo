#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <SDL/SDL.h>

extern SDL_Surface *screen, *layout, *layout_bg, *battery_state;

void load_bg();
void set_transparent();
void load_battery();
void init_video();
void update_video();
void close_video();

#endif // _VIDEO_H_