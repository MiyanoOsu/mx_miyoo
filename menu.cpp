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
    "Change colors",
    "Change buttons",
};

const char *color_title[32] {
    "Red",
    "Green",
    "Blue",
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
    SDL_Color text = {(u8)option.text_red, (u8)option.text_green, (u8)option.text_blue};
    SDL_Color selection = {(u8)option.sel_red, (u8)option.sel_green, (u8)option.sel_blue};
    SDL_Color warning = {255,255,0};
    
    //printf("Before draw: R=%d G=%d B=%d\n", text.r, text.g, text.b);
    
    SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
    for(u8 i = 0; i < max_entry; i++) {
        if(i == section_index)
            draw_string(menu_title[i], layout, 30, 35 + i * 20, selection);
        else
            draw_string(menu_title[i], layout, 30, 35 + i * 20, text);
    }

    if(is_open_setting) {
        draw_string("→",layout, 136, 35 + section_index * 20, selection);
        char buf_backlight[3], buf_volume[3], buf_transparent[4];
        snprintf(buf_backlight, sizeof(buf_backlight), "%u", get_backlight_value());
        snprintf(buf_volume, sizeof(buf_volume), "%u", get_volume_value());
        snprintf(buf_transparent, sizeof(buf_transparent),"%u",option.transparent);
        for(u8 i = 0; i< MAX_SETTING_LIST; i++) {
            if(i == setting_index)
                draw_string(setting_title[i], layout, 160, 35 + i * 20, selection);
            else 
                draw_string(setting_title[i], layout, 160, 35 + i * 20, text);

            if(setting_index == 3)
                draw_string(buf_backlight, layout, 280, 35 + 3 * 20, selection);
            else
                draw_string(buf_backlight, layout, 280, 35 + 3 * 20, text);

            if(setting_index == 4)
                draw_string(buf_volume, layout, 280, 35 + 4 * 20, selection);
            else
                draw_string(buf_volume, layout, 280, 35 + 4 * 20, text);

            if(setting_index == 5)
                draw_string(buf_transparent, layout, 280, 35 + 5 * 20, selection);
            else
                draw_string(buf_transparent, layout, 280, 35 + 5 * 20, text);
                
        }
    }
    
    if(is_open_color_changing) {
        char text_red[4], text_green[4], text_blue[4];
        char sel_red[4], sel_green[4], sel_blue[4];

        snprintf(text_red, sizeof(text_red),"%u",option.text_red);
        snprintf(text_green, sizeof(text_green),"%u",option.text_green);
        snprintf(text_blue, sizeof(text_blue),"%u",option.text_blue);
        snprintf(sel_red, sizeof(sel_red),"%u",option.sel_red);
        snprintf(sel_green, sizeof(sel_green),"%u",option.sel_green);
        snprintf(sel_blue, sizeof(sel_blue),"%u",option.sel_blue);

        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        draw_string("Color Text", layout, 55, 20, text);
        for(u8 i = 1; i < 4; i ++) {
            if(i - 1 == color_index)
                draw_string(color_title[i-1], layout, 30, 20 + i * 20, selection);
            else 
                draw_string(color_title[i-1], layout, 30, 20 + i * 20, text);
            
            if (color_index == 0)
                draw_string(text_red, layout, 140, 20 + 1 * 20, selection);
            else
                draw_string(text_red, layout, 140, 20 + 1 * 20, text);

            if (color_index == 1)
                draw_string(text_green, layout, 140, 20 + 2 * 20, selection);
            else
                draw_string(text_green, layout, 140, 20 + 2 * 20, text);

            if (color_index == 2)
                draw_string(text_blue, layout, 140, 20 + 3 * 20, selection);
            else
                draw_string(text_blue, layout, 140, 20 + 3 * 20, text);
        }
        draw_string("Color Selection", layout, 55, 35 + 4 * 20, text);
        for(u8 i = 5; i < 8; i ++) {
            if(i - 2 == color_index)
                draw_string(color_title[i-5], layout, 30, 35 + i * 20, selection);
            else 
                draw_string(color_title[i-5], layout, 30, 35 + i * 20, text);
            
            if (color_index == 3)
                draw_string(sel_red, layout, 140, 35 + 5 * 20, selection);
            else
                draw_string(sel_red, layout, 140, 35 + 5 * 20, text);
            if (color_index == 4)
                draw_string(sel_green, layout, 140, 35 + 6 * 20, selection);
            else
                draw_string(sel_green, layout, 140, 35 + 6 * 20, text);
            if (color_index == 5)
                draw_string(sel_blue, layout, 140, 35 + 7 * 20, selection);
            else
                draw_string(sel_blue, layout, 140, 35 + 7 * 20, text);
        }
    }

    if(is_open_remap) {
        char key_text[30];
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        s8 end = remap_offset + MAX_VISIBLE_LIST - 1;
        if(remap_index < remap_offset)
            remap_offset = remap_index;
        if (remap_index >= end)
            remap_offset = remap_index - MAX_VISIBLE_LIST + 2;
        
        if(remap_offset + MAX_VISIBLE_LIST - 1 < 14)
            draw_string("↓", layout, 5, 50, selection);
        if(offset > 0)
            draw_string("↑", layout, 15, 50, selection);

        for(u8 i = remap_offset; i < end; i++) {
            snprintf(key_text,sizeof(key_text),remap[i],return_text_button(option.buttons[i]));
            if(i == remap_index)
                draw_string(key_text, layout , 35, 30 + (i - remap_offset) * 20, selection);
            else 
                draw_string(key_text, layout, 35, 30 + (i - remap_offset) * 20, text);
        }
        if(message_waiting) {
            draw_string("Press a key to map...",layout,35,0, warning);
        }
    }

    if(is_open_link) {
        draw_string("→",layout, 136, 35 + section_index * 20, selection);
        if(is_empty_link)
            draw_string("<empty>",layout, 160, 35, text);
        else {
            s16 end = link_offset + MAX_VISIBLE_LIST - 2;
            if(link_index < link_offset)
                link_offset = link_index;
            if (link_index >= end)
                link_offset = link_index - MAX_VISIBLE_LIST + 2 + 1;
            
            if(link_offset + MAX_VISIBLE_LIST - 2 < max_link)
                draw_string("↓", layout, 5, 50, selection);
            if(link_offset > 0)
                draw_string("↑", layout, 15, 50, selection);
            if(max_link < MAX_VISIBLE_LIST - 2) {
                link_offset = 0;
                end = max_link;
            }
            for(u8 i = link_offset; i < end; i++) {
                if(i == link_index)
                    draw_string(list_link[i], layout, 160, 35 + (i - link_offset) * 20, selection);
                else
                    draw_string(list_link[i], layout, 160, 35 + (i - link_offset) * 20, text);
            }
        }
    }

    if(is_open_rom) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        if(is_empty_rom_folder)
            draw_string("<empty>",layout, 60, 35, text);
        else {
            s32 end = offset + MAX_VISIBLE_LIST;
            if(rom_index < offset)
                offset = rom_index;
            if (rom_index >= end)
                offset = rom_index - MAX_VISIBLE_LIST + 1;
            
            if(offset + MAX_VISIBLE_LIST < max_rom_list)
                draw_string("↓", layout, 5, 50, selection);
            if(offset > 0)
                draw_string("↑", layout, 15, 50, selection);
            if(max_rom_list < MAX_VISIBLE_LIST) {
                offset = 0;
                end = max_rom_list;
            }
            for(u16 i = offset; i < end; i++) {
                if(i == rom_index)
                    draw_string(list_rom[i], layout, 35, 20 + (i - offset) * 20, selection);
                else
                    draw_string(list_rom[i], layout, 35, 20 + (i - offset) * 20, text);
            }
        }
    }

    if(is_open_file_list) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        if(is_empty_folder_file)
            draw_string("<empty>",layout, 60, 35, text);
        else {
            s16 end = file_list_offset + MAX_VISIBLE_LIST;
            if(file_list_index < file_list_offset)
                file_list_offset = file_list_index;
            if (file_list_index >= end)
                file_list_offset = file_list_index - MAX_VISIBLE_LIST + 1;
            
            if(file_list_offset + MAX_VISIBLE_LIST < max_file_list)
                draw_string("↓", layout, 5, 50, selection);
            if(file_list_offset > 0)
                draw_string("↑", layout, 15, 50, selection);
            if(max_file_list < MAX_VISIBLE_LIST) {
                file_list_offset = 0;
                end = max_file_list;
            }
            for(u8 i = file_list_offset; i < end; i++) {
                if(i == file_list_index)
                    draw_string(list_file[i], layout, 35, 20 + (i - file_list_offset) * 20, selection);
                else
                    draw_string(list_file[i], layout, 35, 20 + (i - file_list_offset) * 20, text);
            }
        }
    }

    if(is_open_install) {
        SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,0,0,0));
        for(u8 i = 0; i < current_line;i++)
            draw_string(install_info[i],layout,20,20 + i*20, text);

        if(install_done)
            draw_string("Process done, Press B to exit.",layout,20,20 + (current_line)*20, text);
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
