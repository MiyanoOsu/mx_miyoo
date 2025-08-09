#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "common.h"

typedef struct {
	u16 buttons[14];
    char bg[128];
    char font[256];
    s16 transparent;
    s16 text_red;
    s16 text_green;
    s16 text_blue;
    s16 sel_red;
    s16 sel_green;
    s16 sel_blue;
} t_config;

extern t_config option;

extern char home_path[256], config_path[256];
extern u8 done;

void init_config();
void save_config();
void load_config();

#endif // _CONFIG_H_