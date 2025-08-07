#ifndef _MENU_H_
#define _MENU_H_

#include "common.h"

extern s16 link_offset;
extern s32 offset;
extern s16 file_list_offset;

extern const char* menu_title[];
extern char* list_name[];
extern char* list_link[];
extern char* list_rom[];
extern char* list_file[];

extern char background_path[];
extern char font_path[];

#define MAX_SETTING_LIST 7

void draw_menu();

#endif // _MENU_H_