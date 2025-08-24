#include "common.h"
#include "config.h"
#include "input.h"
#include "menu.h"
#include "mx.h"
#include <SDL/SDL.h>

char home_path[256], config_path[256];

void init_config() {
    snprintf(home_path, sizeof(home_path), "%s/mx", getenv("HOME"));
    snprintf(config_path, sizeof(config_path), "mx.cfg");
    if (access("fonts", F_OK ) == -1) mkdir("fonts",0755);
    if (access("background", F_OK ) == -1) mkdir("background",0755);
    if (access("sections", F_OK ) == -1) mkdir("sections",0755);
    if (access("install", F_OK ) == -1) mkdir("install",0755);
    if (access("battery", F_OK ) == -1) mkdir("battery",0755);
    load_config();
}

void save_config() {
	FILE* fp;
	fp = fopen(config_path, "wb");
	if (fp)
	{
		fwrite(&option, sizeof(option), sizeof(int8_t), fp);
		fclose(fp);
	}
}

void load_config() {
    FILE *fp;
    fp = fopen(config_path,"rb");
    if(fp) {
		fread(&option, sizeof(option), sizeof(int8_t), fp);
		fclose(fp);
    } else {
        option.buttons[0] = SDLK_UP;
        option.buttons[1] = SDLK_DOWN;
        option.buttons[2] = SDLK_LEFT;
        option.buttons[3] = SDLK_RIGHT;
        option.buttons[4] = SDLK_LALT;
        option.buttons[5] = SDLK_LCTRL;
        option.buttons[6] = SDLK_LSHIFT;
        option.buttons[7] = SDLK_SPACE;
        option.buttons[8] = SDLK_BACKSPACE;
        option.buttons[9] = SDLK_TAB;
        option.buttons[10] = SDLK_RETURN;
        option.buttons[11] = SDLK_ESCAPE;
        option.buttons[12] = SDLK_RCTRL;
        option.buttons[13] = SDLK_RALT;
        load_default_bg();
        strcpy(option.bg, background_path);
        load_default_font();
        strcpy(option.font, font_path);
        option.font_size = 16;
        option.transparent = 190;
        option.text_red = 255;
        option.text_green = 255;
        option.text_blue = 255;
        option.sel_red = 0;
        option.sel_green = 255;
        option.sel_blue = 255;
        save_config();
    }
}
