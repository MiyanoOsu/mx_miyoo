#ifndef _MX_H_
#define _MX_H_

extern u8 max_entry;
extern u8 max_link;
extern u16 max_rom_list;
extern u16 max_file_list;
extern u8 current_line;
extern u8 done_massage;

void add_section(const char *name);
void load_section();
void load_list_app(u8 order);
void clear_list_app();
void load_rom_list();
void clear_rom_list();
void get_command();
void run_command();
void clear_file_list();
void clear_install_info();
void install_ipk();
void uninstall_ipk();
void load_install_list();
void load_bg_list();
void load_default_bg();
void set_bg();
void load_font_list();
void load_default_font();
void set_font();

u8 get_battery_status();
u8 get_backlight_value();
void set_backlight_value(u8 val);
u8 get_volume_value();
void set_volume_value(u8 val);
void set_CPU(u8 values);

#endif // _MX_H_