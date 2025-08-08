#include "common.h"
#include "input.h" 
#include "config.h"
#include "menu.h"
#include "video.h"
#include "font.h"
#include <dirent.h>
#include <sys/ioctl.h>
#include <libgen.h>

#define MIYOO_BATTERY   "/sys/class/power_supply/miyoo-battery/voltage_now"
#define MIYOO_LID_CONF  "/sys/class/backlight/backlight/brightness"
#define MIYOO_SND_FILE  "/dev/miyoo_snd"
#define MIYOO_SND_GET_VOLUME  _IOWR(0x101, 0, unsigned long)
#define MIYOO_SND_SET_VOLUME  _IOWR(0x100, 0, unsigned long)

u8 max_entry = 2;
u8 max_link = 0;
u16 max_rom_list = 0;
u8 have_load_folder = 0;
u16 max_file_list = 0;

u8 current_line = 0;

char command[512];
char dir[256];
char background_path[128];
char font_path[256];
char binary_path [256];

char **get_section_name(u8 *out_count) {
    struct dirent *directory_entry;  // Pointer for directory entry
    char** section_list = (char**)malloc(sizeof(char*) * 64);
    // opendir() returns a pointer of DIR type. 
    DIR *directory = opendir("sections");

    if (directory == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return NULL;
    }
    u8 count = 0;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0
        || strcmp(directory_entry->d_name, "..") == 0)
            continue;
        if(directory_entry->d_type == DT_DIR) {
            section_list[count++] = strdup(directory_entry->d_name);
            max_entry++;
        }
    }
    closedir(directory);
    
    // sort alphabet
    qsort(section_list, count, sizeof(char *), [](const void *a, const void *b) {
        return strcmp(*(const char **)a, *(const char **)b);
    });

    *out_count = count;
    return section_list;
}

void add_section(const char *name) {
    char folder[256];
    snprintf(folder,sizeof(folder),"sections/%s",name);
    if (mkdir(folder, 0755) == 0) {
        printf("succesfully!");
    } else {
        printf("Failed to create folder");
    }
}

void remove_section(const char *name) {
    char folder[256];
    snprintf(folder,sizeof(folder),"sections/%s",name);
    if (rmdir(folder) == 0) {
        printf("succesfully!");
    } else {
        printf("Failed to create folder");
    }
}

void get_list_apps(char * open_directory, u8 *out_count, char **list) {
    struct dirent *directory_entry;  // Pointer for directory entry
    // opendir() returns a pointer of DIR type. 
    DIR *directory = opendir(open_directory);

    if (directory == NULL)  // opendir returns NULL if couldn't open directory
    {
        return;
    }
    u8 count = 0;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0
        || strcmp(directory_entry->d_name, "..") == 0)
            continue;
        if(directory_entry->d_type == DT_REG) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s",open_directory, directory_entry->d_name);
            FILE* file = fopen(path,"r");
            char line[128];
            if (!file) {
                printf("Check path pls");
                return;
            }
            u8 checked = 0;
            if(is_open_rom == 0) {
                while (fgets(line, sizeof(line), file)) {
                    if (line[0] == '\0') break; // exit if file is empty
                    line[strcspn(line, "\n")] = '\0';
                    if(checked) {
                        if (strncmp(line, "title=", 6) == 0) {
                            list_name[count] = strdup(directory_entry->d_name);
                            list[count++] = strdup(line + 6);
                            checked = 0;
                            break;
                        }
                    } else {
                        if (strncmp(line, "exec=", 5) == 0) {
                            struct stat st;
                            if (stat(line + 5, &st) == 0 && S_ISREG(st.st_mode)) {
                                checked = 1;
                                rewind(file);
                                continue;
                            }
                        }
                    }
                }
            }
            else {
                list[count++] = strdup(directory_entry->d_name);
            }
            fclose(file);
        }
    }
    closedir(directory);
    if(is_open_rom == 1) {
        qsort(list, count, sizeof(char *), [](const void *a, const void *b) {
            return strcmp(*(const char **)a, *(const char **)b);
        });
    }
    *out_count = count;
    return;
}

void get_list_file(const char * open_directory, u8 *out_count, char **list) {
    struct dirent *directory_entry;  // Pointer for directory entry
    // opendir() returns a pointer of DIR type. 
    DIR *directory = opendir(open_directory);

    if (directory == NULL)  // opendir returns NULL if couldn't open directory
    {
        return;
    }
    u8 count = 0;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0
        || strcmp(directory_entry->d_name, "..") == 0)
            continue;
        if(directory_entry->d_type == DT_REG) {
            list[count++] = strdup(directory_entry->d_name);
        }
    }
    closedir(directory);
    
    // sort alphabet
    qsort(list, count, sizeof(char *), [](const void *a, const void *b) {
        return strcmp(*(const char **)a, *(const char **)b);
    });

    *out_count = count;
    return;
}

void get_command() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    FILE* file = fopen(path,"r");
    if(!file) {
        printf("path not found");
    }
    char line[128];
    while(fgets(line,sizeof(line),file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line,"exec=", 5) == 0) {
            strcpy(command,".");
            strcat(command, strrchr(line + 5, '/'));
            strcat(command, " ");
            strcpy(binary_path, dirname(line + 5));
        }
        if (strncmp(line,"params=", 7) == 0) {
            strcat(command, line + 7);
            strcat(command, " ");
        }
        if(strncmp(line,"selectordir=", 12) == 0) {
            struct stat st;
            if (stat(line + 12, &st) == 0 && S_ISDIR(st.st_mode)) {
                strcat(command,"\"");
                strcat(command,line + 12);
                strcpy(dir,line + 12);
            }
            have_load_folder = 1;
        }
    }
    fclose(file);
}

void load_section() {
    u8 section_count = 0;
    char **section_name = get_section_name(&section_count);
    for (int i = max_entry - 1; i >= section_count; i--) {
        menu_title[i] = menu_title[i - section_count];
    }

    for (int i = 0; i < section_count; i++) {
        menu_title[i] = strdup(section_name[i]);
    }
    
    for (int i = 0; i < section_count; i++) {
        free(section_name[i]);
    }
    free(section_name);
}

void load_list_app(u8 order) {
    u8 list_count = 0;
    char default_sections[128];
    snprintf(default_sections,sizeof(default_sections),"sections/%s",menu_title[order]);
    get_list_apps(default_sections,&list_count, list_link);
    max_link = list_count;
}

void clear_list_app() {
    for (int i = 0; i < max_link; i++) {
        free(list_link[i]);
        list_link[i] = NULL;
    }
    for (int i = 0; i < max_link; i++) {
        free(list_file[i]);
        list_file[i] = NULL;
    }
    max_link = 0;
    link_index = 0;
}

void load_rom_list() {
    u8 list_count = 0;
    get_list_apps(dir,&list_count,list_rom);
    max_rom_list = list_count;
}

void clear_list_rom() {
    for (int i = 0; i < max_rom_list; i++) {
        free(list_rom[i]);
        list_rom[i] = NULL;
    }
    max_rom_list = 0;
    rom_index = 0;
}

void clear_file_list() {
    for(u16 i = 0; i < max_file_list; i++) {
        free(list_file[i]);
        list_file[i] = NULL;
    }
    max_file_list = 0;
    file_list_index = 0;
    file_list_offset = 0;
    is_empty_folder_file = 0;
}

void clear_install_info() {
    for(u16 i = 0; i < current_line; i++) {
        free(install_info[i]);
        install_info[i] = NULL;
    }
    current_line = 0;
}

void install_ipk() {
    char cmd[512] = "/usr/bin/opkg install --force-reinstall --force-overwrite \"install/";
    strcat(cmd,list_file[file_list_index]);
    strcat(cmd, "\"");
    FILE *fp;
    char buffer[256];
    system("mount -oremount,rw /");
    fp = popen(cmd,"r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        install_info[current_line] = strdup(buffer);
        current_line++;
        update_video();
    }
    pclose(fp);
    system("mount -oremount,ro /");
}

void load_install_list() {
    u8 list_count = 0;
    get_list_file("install",&list_count,list_file);
    max_file_list = list_count;
}

void load_bg_list() {
    u8 list_count = 0;
    get_list_file("background",&list_count,list_file);
    max_file_list = list_count;
}

void load_default_bg() {
    u8 list_count = 0;
    get_list_file("background",&list_count,list_file);
    snprintf(background_path,sizeof(background_path),"background/%s",list_file[0]);
    clear_file_list();
}

void set_bg() {
    u8 list_count = 0;
    get_list_file("background",&list_count,list_file);
    snprintf(background_path,sizeof(background_path),"background/%s",list_file[file_list_index]);
    strcpy(option.bg, background_path);
    save_config();
}

void load_font_list() {
    u8 list_count = 0;
    get_list_file("fonts",&list_count,list_file);
    max_file_list = list_count;
}

void load_default_font() {
    u8 list_count = 0;
    get_list_file("fonts",&list_count,list_file);
    snprintf(font_path,sizeof(font_path),"fonts/%s",list_file[0]);
    clear_file_list();
}

void set_font() {
    u8 list_count = 0;
    get_list_file("fonts",&list_count,list_file);
    snprintf(font_path,sizeof(font_path),"fonts/%s",list_file[file_list_index]);
    strcpy(option.font, font_path);
    save_config();
}

void run_command() {
    if (have_load_folder) {
        strcat(command,"/");
        strcat(command,list_rom[rom_index]);
        strcat(command,"\"");
    }
    chdir(binary_path);
    close_font();
    close_video();
    clear_list_rom();
    clear_list_app();
    clear_file_list();
    done = 1;
    system(command);
}

u8 get_battery_status() {
	int val = -1;
	if (FILE *f = fopen(MIYOO_BATTERY, "r")) {
		fscanf(f, "%i", &val);
		fclose(f);
	}
	if ((val > 4300) || (val < 0)) return 6; // >100% - max voltage 4320
	if (val > 4100) return 5; // 100% - fully charged 4150
	if (val > 3900) return 4; // 80%
	if (val > 3800) return 3; // 60%
	if (val > 3700) return 2; // 40%
	if (val > 3520) return 1; // 20%
	return 0; // 0% :(
}

u8 get_backlight_value() {
    int val = -1;
	if(FILE *f = fopen(MIYOO_LID_CONF, "r")) {
        fscanf(f,"%i",&val);
        fclose(f);
    }
    return val;
}

void set_backlight_value(u8 val) {
    char buf[3] = {0};
    snprintf(buf, sizeof(buf), "%d\n", val);
    if (FILE *f = fopen(MIYOO_LID_CONF, "w")) {
        fputs(buf,f);
        fclose(f);
    }
}

u8 get_volume_value() {
    unsigned long val = -1;
    int snd = open(MIYOO_SND_FILE, O_RDWR);
    if(snd >= 0) {
        ioctl(snd, MIYOO_SND_GET_VOLUME, &val);
		close(snd);
    }
    return (u8)val;
}

void set_volume_value(u8 val) {
    unsigned long volume = val;
    int snd = open(MIYOO_SND_FILE, O_RDWR);
	if (snd >= 0) {
		ioctl(snd, MIYOO_SND_SET_VOLUME, volume);
		close(snd);
	}
}
