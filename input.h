#ifndef _INPUT_H_
#define _INPUT_H_

#include "config.h"

#define BTN_UP      option.buttons[0]
#define BTN_DOWN    option.buttons[1] 
#define BTN_LEFT    option.buttons[2]
#define BTN_RIGHT   option.buttons[3]
#define BTN_A       option.buttons[4]
#define BTN_B       option.buttons[5]
#define BTN_X       option.buttons[6]
#define BTN_Y       option.buttons[7]
#define BTN_R       option.buttons[8]
#define BTN_L       option.buttons[9]
#define BTN_START   option.buttons[10]
#define BTN_SELECT  option.buttons[11]
#define BTN_PLUS    option.buttons[12]
#define BTN_MINUS   option.buttons[13]

extern s8 section_index;
extern s8 link_index;
extern s32 rom_index;
extern s8 setting_index;
extern s16 file_list_index;
extern s8 remap_index;

extern u8 is_open_link;
extern u8 is_empty_link;
extern u8 is_open_rom;
extern u8 is_empty_rom_folder;
extern u8 is_open_setting;
extern u8 is_open_file_list;
extern u8 is_empty_folder_file;
extern u8 have_load_folder;
extern u8 is_open_remap;
extern u8 is_open_install;
extern u8 message_waiting;

void handle_input();


#endif // _INPUT_H_