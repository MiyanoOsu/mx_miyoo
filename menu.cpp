#include "common.h"
#include "font.h"
#include "video.h"
#include "input.h"
#include "menu.h"
#include "mx.h"

const char *menu_title[64] = {"settings","exit"};
const char *setting_title[64] = {
    "Install apps",
    "Change background",
    "Change fonts",
    "Backlight",
    "Volume",
    "Transparent",
    "Change buttons",
   
};

char *list_link[64] = {NULL};
char *list_name[256] = {NULL};
char *list_rom[256] = {NULL};
char *list_file[256] = {NULL};
char *install_info[256] = {NULL};

const char *remap[35] = {
    "UP:  %s",
    "DOWN:  %s",
    "LEFT:  %s",
    "RIGHT:  %s",
    "A:  %s",
    "B:  %s",
    "X:  %s",
    "Y:  %s",
    "R:  %s",
    "L:  %s",
    "START:  %s",
    "SELECT:  %s",
    "PLUS/RESET:  %s",
    "MINUS:  %s"
};

s16 link_offset = 0;
s32 offset = 0;
s16 file_list_offset = 0;
s8 remap_offset = 0;

#define MAX_VISIBLE_LIST 10

const char* return_text_button(u32 button) {
    switch(button) {
        case SDLK_UP:
            return "DPAD UP";
        case SDLK_DOWN:
            return "DPAD DOWN";
        case SDLK_LEFT:
            return "DPAD LEFT";
        case SDLK_RIGHT:
            return "DPAD RIGHT";
        case SDLK_LALT:
            return "BTN A";
        case SDLK_LCTRL:
            return "BTN B";
        case SDLK_LSHIFT:
            return "BTN X";
        case SDLK_SPACE:
            return "BTN Y";
        case SDLK_BACKSPACE:
            return "BTN R";
        case SDLK_TAB:
            return "BTN L";
        case SDLK_RETURN:
            return "START";
        case SDLK_ESCAPE:
            return "SELECT";
        case SDLK_RCTRL:
            return "BTN PLUS/ RESET";
        case SDLK_RALT:
            return "BTN MINUS";
    	case 0:
			return "...";
        default:
            return "UNKNOW";
    }

}

void draw_menu() {
    SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
    for(u8 i = 0; i < max_entry; i++) {
        if(i == section_index)
            draw_string(menu_title[i], layout, 30, 35 + i * 20, 0, 255, 255);
        else
            draw_string(menu_title[i], layout, 30, 35 + i * 20, 255, 255, 255);
    }

    if(is_open_setting) {
        draw_string("→",layout, 136, 35 + section_index * 20, 0, 255, 255);
        char buf_backlight[3], buf_volume[3], buf_transparent[4];
        snprintf(buf_backlight, sizeof(buf_backlight), "%u", get_backlight_value());
        snprintf(buf_volume, sizeof(buf_volume), "%u", get_volume_value());
        snprintf(buf_transparent, sizeof(buf_transparent),"%u",option.transparent);
        for(u8 i = 0; i< MAX_SETTING_LIST; i++) {
            if(i == setting_index)
                draw_string(setting_title[i], layout, 160, 35 + i * 20, 0, 255, 255);
            else {
                draw_string(setting_title[i], layout, 160, 35 + i * 20, 255, 255, 255);
                draw_string(buf_backlight, layout, 280, 35 + 3 * 20, 255, 255, 255);
                draw_string(buf_volume, layout, 280, 35 + 4 * 20, 255, 255, 255);
                draw_string(buf_transparent, layout, 280, 35 + 5 * 20, 255, 255, 255);
            }
            if(setting_index == 3)
                draw_string(buf_backlight, layout, 280, 35 + 3 * 20, 0, 255, 255);
            if(setting_index == 4)
                draw_string(buf_volume, layout, 280, 35 + 4 * 20, 0, 255, 255);
            if(setting_index == 5)
                draw_string(buf_transparent, layout, 280, 35 + 5 * 20, 0, 255, 255);
        }
    }

    if(is_open_remap) {
        char text[30];
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        s8 end = remap_offset + MAX_VISIBLE_LIST - 1;
        if(remap_index < remap_offset)
            remap_offset = remap_index;
        if (remap_index >= end)
            remap_offset = remap_index - MAX_VISIBLE_LIST + 2;
        
        if(remap_offset + MAX_VISIBLE_LIST - 1 < 14)
            draw_string("↓", layout, 5, 50, 0, 255, 255);
        if(offset > 0)
            draw_string("↑", layout, 15, 50, 0, 255, 255);

        for(u8 i = remap_offset; i < end; i++) {
            snprintf(text,sizeof(text),remap[i],return_text_button(option.buttons[i]));
            if(i == remap_index)
                draw_string(text, layout , 35, 25 + (i - remap_offset) * 20, 0, 255, 255);
            else 
                draw_string(text, layout, 35, 25 + (i - remap_offset) * 20, 255, 255, 255);
        }
        if(message_waiting) {
            draw_string("Press a key to map...",layout,35,0,255,255,0);
        }
    }

    if(is_open_link) {
        draw_string("→",layout, 136, 35 + section_index * 20, 0, 255, 255);
        if(is_empty_link)
            draw_string("<empty>",layout, 160, 35, 255, 255, 255);
        else {
            s16 end = link_offset + MAX_VISIBLE_LIST - 2;
            if(link_index < link_offset)
                link_offset = link_index;
            if (link_index >= end)
                link_offset = link_index - MAX_VISIBLE_LIST + 2 + 1;
            
            if(link_offset + MAX_VISIBLE_LIST - 2 < max_link)
                draw_string("↓", layout, 5, 50, 0, 255, 255);
            if(link_offset > 0)
                draw_string("↑", layout, 15, 50, 0, 255, 255);
            if(max_link < MAX_VISIBLE_LIST - 2) {
                link_offset = 0;
                end = max_link;
            }
            for(u8 i = link_offset; i < end; i++) {
                if(i == link_index)
                    draw_string(list_link[i], layout, 160, 35 + (i - link_offset) * 20, 0, 255, 255);
                else
                    draw_string(list_link[i], layout, 160, 35 + (i - link_offset) * 20, 255, 255, 255);
            }
        }
    }

    if(is_open_rom) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        if(is_empty_rom_folder)
            draw_string("<empty>",layout, 60, 35, 255, 255, 255);
        else {
            s32 end = offset + MAX_VISIBLE_LIST;
            if(rom_index < offset)
                offset = rom_index;
            if (rom_index >= end)
                offset = rom_index - MAX_VISIBLE_LIST + 1;
            
            if(offset + MAX_VISIBLE_LIST < max_rom_list)
                draw_string("↓", layout, 5, 50, 0, 255, 255);
            if(offset > 0)
                draw_string("↑", layout, 15, 50, 0, 255, 255);
            if(max_rom_list < MAX_VISIBLE_LIST) {
                offset = 0;
                end = max_rom_list;
            }
            for(u16 i = offset; i < end; i++) {
                if(i == rom_index)
                    draw_string(list_rom[i], layout, 35, 20 + (i - offset) * 20, 0, 255, 255);
                else
                    draw_string(list_rom[i], layout, 35, 20 + (i - offset) * 20, 255, 255, 255);
            }
        }
    }

    if(is_open_file_list) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        if(is_empty_folder_file)
            draw_string("<empty>",layout, 60, 35, 255, 255, 255);
        else {
            s16 end = file_list_offset + MAX_VISIBLE_LIST;
            if(file_list_index < file_list_offset)
                file_list_offset = file_list_index;
            if (file_list_index >= end)
                file_list_offset = file_list_index - MAX_VISIBLE_LIST + 1;
            
            if(file_list_offset + MAX_VISIBLE_LIST < max_file_list)
                draw_string("↓", layout, 5, 50, 0, 255, 255);
            if(file_list_offset > 0)
                draw_string("↑", layout, 15, 50, 0, 255, 255);
            if(max_file_list < MAX_VISIBLE_LIST) {
                file_list_offset = 0;
                end = max_file_list;
            }
            for(u8 i = file_list_offset; i < end; i++) {
                if(i == file_list_index)
                    draw_string(list_file[i], layout, 35, 20 + (i - file_list_offset) * 20, 0, 255, 255);
                else
                    draw_string(list_file[i], layout, 35, 20 + (i - file_list_offset) * 20, 255, 255, 255);
            }
        }
    }

    if(is_open_install) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        for(u8 i = 0; i < current_line;i++)
            draw_string(install_info[i],layout,20,20 + i*20,255,255,255);
    }

    SDL_BlitSurface(battery_state, NULL, layout,[](){
        static SDL_Rect r = {301, 2, 16, 16};
        return &r;
    }());

    // SDL_FillRect(layout_bg, [](){
    //     static SDL_Rect r = {0, 220, 320, 20};
    //     return &r;
    // }(), SDL_MapRGB(layout_bg->format ,0 ,255 ,255));
    // draw_string("A: Choose   B: Back",layout_bg, 170, 216, 0, 0, 0);
}
