#include "common.h"
#include "config.h"
#include "input.h"
#include "video.h"
#include "font.h"
#include "menu.h"
#include "mx.h"
#include <SDL/SDL.h>

t_config option;
s8 section_index = 0;
s8 link_index = 0;
s32 rom_index = 0;
s16 file_list_index = 0;
s8 setting_index = 0;
s8 remap_index = 0;
s8 color_index = 0;

u8 is_open_link = 0;
u8 is_empty_link = 0;
u8 is_open_rom = 0;
u8 is_empty_rom_folder = 0;
u8 is_open_setting = 0;
u8 is_open_file_list = 0;
u8 is_empty_folder_file = 0;
u8 is_open_remap = 0;
u8 is_open_install = 0;
u8 message_waiting = 0;
u8 is_open_color_changing = 0;

SDLKey wait_for_key_input() {
    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                message_waiting = 0;
                return event.key.keysym.sym;
            }
            if (event.type == SDL_QUIT) {
                exit(0);
            }
        }
        SDL_Delay(10); // prevent busy loop
    }
}

void handle_input() {
    SDL_Event event;
    while(SDL_PollEvent(&event)){

        if(message_waiting)
            option.buttons[remap_index] = wait_for_key_input();

        switch(event.type) {
            case SDL_QUIT:
                done = 1;
            break;
			case SDL_KEYDOWN:
                if (event.key.keysym.sym == BTN_UP) {
                    if(is_open_link == 1 && is_open_rom == 0) {
                        link_index--;
                        if(link_index < 0) link_index = max_link-1;
                    } else if(is_open_file_list) {
                        file_list_index--;
                        if(file_list_index < 0)
                            file_list_index = max_file_list - 1;
                    } else if(is_open_setting && is_open_color_changing) {
                        color_index--;
                        if(color_index < 0)
                            color_index = 5;
                    } else if(is_open_setting && is_open_remap) {
                        remap_index--;
                        if(remap_index < 0)
                            remap_index = 14-1;
                    } else if(is_open_setting) {
                        setting_index--;
                        if(setting_index < 0)
                            setting_index = MAX_SETTING_LIST -1;
                    } else if(is_open_rom) {
                        rom_index--;
                        if(rom_index < 0) rom_index = max_rom_list-1;
                    } else {
                        section_index--;
                        if(section_index < 0) section_index = max_entry-1;
                    }
                }
                if (event.key.keysym.sym == BTN_DOWN) {
                    if(is_open_link == 1 && is_open_rom == 0) {
                        link_index++;
                        if(link_index > max_link-1) link_index = 0;
                    } else if(is_open_file_list) {
                        file_list_index++;
                        if(file_list_index > max_file_list - 1)
                            file_list_index = 0;
                    } else if(is_open_setting && is_open_color_changing) {
                        color_index++;
                        if(color_index > 5)
                            color_index = 0;
                    } else if(is_open_setting && is_open_remap) {
                        remap_index++;
                        if(remap_index > 14-1)
                            remap_index = 0;
                    }else if(is_open_setting) {
                        setting_index++;
                        if(setting_index > MAX_SETTING_LIST - 1)
                            setting_index = 0;
                    } else if(is_open_rom) {
                        rom_index++;
                        if(rom_index>max_rom_list-1) rom_index = 0;
                    } else {
                        section_index++;
                        if(section_index > max_entry-1) section_index = 0;
                    }
                }
                if (event.key.keysym.sym == BTN_LEFT) {
                    if(is_open_setting == 1 && setting_index == 3) {
                        u8 val = get_backlight_value();
                        val--;
                        if (val < 1) val = 1;
                        set_backlight_value(val);
                    } else if(is_open_setting == 1 && setting_index == 4) {
                        u8 val = get_volume_value();
                        val--;
                        if (val < 1) val = 1;
                        set_volume_value(val);
                    } else if(is_open_setting == 1 && setting_index == 5) {
                        option.transparent-=4;
                        if (option.transparent < 0) option.transparent = 0;
                        set_transparent();
                        save_config();
                    } else if(is_open_setting == 1 && is_open_color_changing == 1) {
                        if(color_index == 0) {
                            option.text_red-=1;
                            if(option.text_red < 0)
                                option.text_red = 255;
                        } else if(color_index == 1) {
                            option.text_green-=1;
                            if(option.text_green < 0)
                                option.text_green = 255;
                        } else if(color_index == 2) {
                            option.text_blue-=1;
                            if(option.text_blue < 0)
                                option.text_blue = 255;
                        } else if(color_index == 3) {
                            option.sel_red-=1;
                            if(option.sel_red < 0)
                                option.sel_red = 255;
                        } else if(color_index == 4) {
                            option.sel_green-=1;
                            if(option.sel_green < 0)
                                option.sel_green = 255;
                        } else if(color_index == 255) {
                            option.sel_blue-=1;
                            if(option.sel_blue < 0)
                                option.sel_blue = 255;
                        }
                    }
                }
                if (event.key.keysym.sym == BTN_RIGHT) {
                    if(is_open_setting == 1 && setting_index == 3) {
                        u8 val = get_backlight_value();
                        val++;
                        if (val > 9) val = 9;
                        set_backlight_value(val);
                    } else if(is_open_setting == 1 && setting_index == 4) {
                        u8 val = get_volume_value();
                        val++;
                        if (val > 9) val = 9;
                        set_volume_value(val);
                    } else if(is_open_setting == 1 && setting_index == 5) {
                        option.transparent+=4;
                        if (option.transparent > 255) option.transparent = 255;
                        set_transparent();
                        save_config();
                    } else if(is_open_setting == 1 && is_open_color_changing == 1) {
                        if(color_index == 0) {
                            option.text_red+=1;
                            if(option.text_red > 255)
                                option.text_red = 0;
                        } else if(color_index == 1) {
                            option.text_green+=1;
                            if(option.text_green > 255)
                                option.text_green = 0;
                        } else if(color_index == 2) {
                            option.text_blue+=1;
                            if(option.text_blue > 255)
                                option.text_blue = 0;
                        } else if(color_index == 3) {
                            option.sel_red+=1;
                            if(option.sel_red > 255)
                                option.sel_red = 0;
                        } else if(color_index == 4) {
                            option.sel_green+=1;
                            if(option.sel_green > 255)
                                option.sel_green = 0;
                        } else if(color_index == 5) {
                            option.sel_blue+=1;
                            if(option.sel_blue > 255)
                                option.sel_blue = 0;
                        }
                    }
                }
                if (event.key.keysym.sym == BTN_A) {
                    if(section_index == max_entry-1) {
                        done = 1;
                        //system("sync;poweroff"); //uncomment to make devices turn off when press exitss
                    } else if(is_open_setting == 1 && setting_index == 0 && is_open_file_list == 1 && is_open_install == 0) {
                        is_open_install = 1;
                        install_ipk();
                    } else if(is_open_setting == 1 && setting_index == 0) {
                        is_open_file_list = 1;
                        load_install_list();
                        if(max_file_list == 0) {
                            is_empty_folder_file = 1;
                        }
                    } else if(is_open_setting == 1 && setting_index == 1 && is_open_file_list == 1) {
                        set_bg();
                        load_bg();
                    } else if(is_open_setting == 1 && setting_index == 2 && is_open_file_list == 1) {
                        set_font();
                        load_font();
                    } else if(is_open_setting == 1 && setting_index == 7 && is_open_remap == 1) {
                        option.buttons[remap_index] = 0;
                        message_waiting = 1;
                    } else if(is_open_setting == 1 && setting_index == 1) {
                        is_open_file_list = 1;
                        load_bg_list();
                    } else if(is_open_setting == 1 && setting_index == 2) {
                        is_open_file_list = 1;
                        load_font_list();
                    } else if(is_open_setting == 1 && setting_index == 6) {
                        is_open_color_changing = 1;
                    } else if(is_open_setting == 1 && setting_index == 7) {
                        is_open_remap = 1;
                    } else if(section_index == max_entry - 2) {
                        is_open_setting = 1;
                    } else if(is_empty_link) {}
                    else {
                        if(is_open_link == 0) {
                            is_open_link = 1;
                            load_list_app(section_index);
                            if(max_link == 0 && section_index != max_entry - 2)
                                is_empty_link = 1;
                        } else if(is_open_link == 1 && is_open_rom == 0 && is_open_install == 0) {
                            get_command();
                            if(have_load_folder == 1) {
                                is_open_rom = 1;
                                load_rom_list();
                                if (max_rom_list == 0)
                                    is_empty_rom_folder = 1;
                            } else {
                                run_command();
                            }
                        } else if(is_empty_rom_folder) {
                        } else if(is_open_install == 0){
                            run_command();
                        }
                    }
                }
                if (event.key.keysym.sym == BTN_B) {
                    if(is_open_install == 1) {
                        is_open_install = 0;
                        clear_install_info();
                        clear_list_app();
                        load_list_app(section_index);
                        if(max_link == 0 && section_index != max_entry - 2)
                            is_empty_link = 1;
                    } else if(is_open_link == 1 && is_open_rom == 0 ) {
                        is_open_link = 0;
                        clear_list_app();
                        link_index = 0;
                        if(max_link == 0 && section_index != max_entry - 2)
                            is_empty_link = 0;
                    } else if(is_open_file_list == 1) {
                        is_open_file_list = 0;
                        clear_file_list();
                    } else if(is_open_setting == 1 && is_open_remap == 1) {
                        is_open_remap = 0;
                        remap_index = 0;
                        load_config();
                    } else if(is_open_setting == 1 && is_open_color_changing == 1) {
                        is_open_color_changing = 0;
                        init_config();
                    } else if(section_index == max_entry - 2) is_open_setting = 0;
                    else if(is_open_rom == 1 && have_load_folder == 1){
                        is_open_rom = 0;
                        have_load_folder = 0;
                        clear_list_rom();
                        if(max_rom_list == 0)
                            is_empty_rom_folder = 0;
                    }
                }
                if (event.key.keysym.sym == BTN_L) {
                    if(is_open_link == 1 && is_open_rom == 0) {
                        link_index-=7;
                        if(link_index < 0) link_index = 0;
                    } else if(is_open_setting == 1 && is_open_color_changing == 1) {
                        if(color_index == 0) {
                            option.text_red-=10;
                            if(option.text_red < 0)
                                option.text_red = 255;
                        } else if(color_index == 1) {
                            option.text_green-=10;
                            if(option.text_green < 0)
                                option.text_green = 255;
                        } else if(color_index == 2) {
                            option.text_blue-=10;
                            if(option.text_blue < 0)
                                option.text_blue = 255;
                        } else if(color_index == 3) {
                            option.sel_red-=10;
                            if(option.sel_red < 0)
                                option.sel_red = 255;
                        } else if(color_index == 4) {
                            option.sel_green-=10;
                            if(option.sel_green < 0)
                                option.sel_green = 255;
                        } else if(color_index == 255) {
                            option.sel_blue-=10;
                            if(option.sel_blue < 0)
                                option.sel_blue = 255;
                        }
                    } else if(is_open_file_list) {
                        file_list_index-=9;
                        if(file_list_index < 0)
                            file_list_index = 0;
                    }else if(is_open_setting) {
                        setting_index-=9;
                        if(setting_index < 0)
                            setting_index = 0;
                    } else if(is_open_rom) {
                        rom_index-=9;
                        if(rom_index < 0) rom_index = 0;
                    } else {
                        section_index-=9;
                        if(section_index < 0) section_index = 0;
                    }
                }
                if (event.key.keysym.sym == BTN_R) {
                    if(is_open_link == 1 && is_open_rom == 0) {
                        link_index+=7;
                        if(link_index > max_link-1) link_index = max_link-1;
                    } else if(is_open_setting == 1 && is_open_color_changing == 1) {
                        if(color_index == 0) {
                            option.text_red+=10;
                            if(option.text_red > 255)
                                option.text_red = 0;
                        } else if(color_index == 1) {
                            option.text_green+=10;
                            if(option.text_green > 255)
                                option.text_green = 0;
                        } else if(color_index == 2) {
                            option.text_blue+=10;
                            if(option.text_blue > 255)
                                option.text_blue = 0;
                        } else if(color_index == 3) {
                            option.sel_red+=10;
                            if(option.sel_red > 255)
                                option.sel_red = 0;
                        } else if(color_index == 4) {
                            option.sel_green+=10;
                            if(option.sel_green > 255)
                                option.sel_green = 0;
                        } else if(color_index == 5) {
                            option.sel_blue+=10;
                            if(option.sel_blue > 255)
                                option.sel_blue = 0;
                        }
                    } else if(is_open_file_list) {
                        file_list_index+=9;
                        if(file_list_index > max_file_list - 1)
                            file_list_index = max_file_list - 1;
                    }else if(is_open_setting) {
                        setting_index+=9;
                        if(setting_index > MAX_SETTING_LIST - 1)
                            setting_index = MAX_SETTING_LIST - 1;
                    } else if(is_open_rom) {
                        rom_index+=9;
                        if(rom_index>max_rom_list-1) rom_index = max_rom_list - 1;
                    } else {
                        section_index+=9;
                        if(section_index > max_entry-1) section_index = max_entry-1;
                    }
                }
                if (event.key.keysym.sym == BTN_START) {
                    if(is_open_setting == 1 && is_open_remap == 1) {
                        save_config();
                        is_open_remap = 0;
                    } else if (is_open_setting && is_open_color_changing == 1) {
                        save_config();
                    }
                    
                }
                if (event.key.keysym.sym == BTN_SELECT) {
                    if(is_open_link == 1) {
                        is_open_install = 1;
                        uninstall_ipk();
                    }
                }
            break;
        }
    }
}