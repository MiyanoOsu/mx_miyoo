#include "common.h"
#include "input.h" 
#include "config.h"
#include "menu.h"
#include "video.h"
#include "font.h"
#include <dirent.h>
#include <sys/ioctl.h>
#include <libgen.h>
#include <sys/mount.h>

#define MIYOO_BATTERY   "/sys/class/power_supply/miyoo-battery/voltage_now"
#define MIYOO_LID_CONF  "/sys/class/backlight/backlight/brightness"
#define MIYOO_SND_FILE  "/dev/miyoo_snd"
#define MIYOO_SND_GET_VOLUME  _IOWR(0x101, 0, unsigned long)
#define MIYOO_SND_SET_VOLUME  _IOWR(0x100, 0, unsigned long)

u8 max_entry = 2;
u8 max_link = 0;
u16 max_rom_list = 0;
u16 max_file_list = 0;

u8 current_line = 0;
u8 done_massage = 0;

char command[512];
char dir[256];
char background_path[128];
char font_path[256];
char binary_path [256];

char *package_name[256] = {NULL};

char **get_section_name(u8 *out_count) {
    struct dirent *directory_entry;
    char** section_list = (char**)malloc(sizeof(char*) * 64);

    DIR *directory = opendir("sections");

    if (directory == NULL)
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

void get_list(char * open_directory, u8 *out_count, char **list) {
    struct dirent *directory_entry;
    
    DIR *directory = opendir(open_directory);

    if (directory == NULL)
    {
        return;
    }
    u8 count = 0;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0)
            continue;

        list[count++] = strdup(directory_entry->d_name);
    }
    closedir(directory);
    qsort(list, count, sizeof(char *), [](const void *a, const void *b) {
        return strcmp(*(const char **)a, *(const char **)b);
    });
    *out_count = count;
    return;
}

void get_list_apps(char * open_directory, u8 *out_count, char **list) {
    struct dirent *directory_entry;

    DIR *directory = opendir(open_directory);

    if (directory == NULL)
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
            char line[256];
            if (!file) {
                printf("Check path pls");
                return;
            }
            u8 checked = 0;
            while (fgets(line, sizeof(line), file)) {
                if (line[0] == '\0') break; // exit if file is empty
                line[strcspn(line, "\n\r")] = '\0';
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
                            package_name[count] = strdup(line+5);
                            checked = 1;
                            rewind(file);
                            continue;
                        }
                    }
                }
            }
            fclose(file);
        }
    }
    closedir(directory);
    *out_count = count;
    return;
}

void get_list_file(const char * open_directory, u8 *out_count, char **list) {
    struct dirent *directory_entry;

    DIR *directory = opendir(open_directory);

    if (directory == NULL)
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

u8 check_access_folder() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    
    FILE* file = fopen(path,"r");
    if(!file) {
        printf("path not found");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if(strncmp(line,"selectordir=", 12) == 0) {
            return 1;
        }
    }

    return 0;
}

void enable_access_folder() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    
    FILE* file = fopen(path,"a");
    if(!file) {
        perror("path not found");
        return;
    }
    
    const char *line = "selectordir=/mnt";
    if (fputs(line, file) == EOF) {
        perror("Error writing to file");
    }
    
    fclose(file);
}

void remove_access_folder() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    FILE* file = fopen(path,"r");
    if(!file) {
        printf("path not found");
        return;
    }
    
    char lines[256][256];
    int count = 0;
    while(fgets(lines[count],sizeof(lines[count]),file)) {
        if(strncmp(lines[count],"selectordir=", 12) != 0) {
            count++;
        }
    }
    fclose(file);
    
    file = fopen(path,"w");
    for (int i = 0; i < count; i++) {
        fputs(lines[i], file);
    }

    fclose(file);
}

void get_command() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    FILE* file = fopen(path,"r");
    if(!file) {
        printf("path not found");
        return;
    }
    char line[128];
    command[0] = '\0';
    binary_path[0] = '\0';
    dir[0] = '\0';
    while(fgets(line,sizeof(line),file)) {
        line[strcspn(line, "\n\r")] = '\0';
        if (strncmp(line,"exec=", 5) == 0) {
            char execpath[256];
            strncpy(execpath, line + 5, sizeof(execpath) - 1);
            execpath[sizeof(execpath) - 1] = '\0';

            // extract basename and dirname safely
            char *base = basename(execpath);
            char *dirn = dirname(execpath);

            snprintf(command, sizeof(command), "./%s ", base);
            snprintf(binary_path, sizeof(binary_path), "%s", dirn);
        }
        if (strncmp(line,"params=", 7) == 0) {
            size_t len = strlen(command);
            snprintf(command + len, sizeof(command) - len, "%s ", line + 7);
        }
        if(strncmp(line,"selectordir=", 12) == 0) {
            struct stat st;
            if (stat(line + 12, &st) == 0 && S_ISDIR(st.st_mode)) {
                snprintf(dir, sizeof(dir), "%s", line + 12);
            } else {
                snprintf(dir, sizeof(dir), "%s", getenv("HOME"));
            }
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
    is_open_section = 1;
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
    get_list(dir,&list_count,list_rom);
    max_rom_list = list_count;
}

void clear_rom_list() {
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
    done_massage = 0;
}

void scanning(const char *path, char **list, u16 *count) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("Error when open directory");
        return;
    }
    
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // found directory → recurse
            scanning(fullpath, list, count);
        } else {
            // found file, add to array
            list[*count] = strdup(fullpath);
            (*count)++;
        }
    }

    closedir(dp);
    return;
}

#include <sys/sendfile.h>

void copy_file(const char *src, const char *dst) {
    int src_fd, dst_fd;
    struct stat stat_buf;
    off_t offset = 0;

    // open source file
    src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        perror("Error when open source file");
        return;
    }

    // get size of source file
    if (fstat(src_fd, &stat_buf) < 0) {
        perror("fstat get size failed");
        close(src_fd);
        return;
    }

    // open destination file
    dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
    if (dst_fd < 0) {
        perror("Failed to open destination file");
        close(src_fd);
        return;
    }

    // copy using sendfile
    if (sendfile(dst_fd, src_fd, &offset, stat_buf.st_size) < 0) {
        perror("Error when sending file");
        close(src_fd);
        close(dst_fd);
        return;
    }

    close(src_fd);
    close(dst_fd);
    return;
}

char *short_path(const char *fullpath) {
    static char result[128];
    char pathcopy1[128];
    char pathcopy2[128];

    // make two separate copies because dirname() and basename() modify them
    strncpy(pathcopy1, fullpath, sizeof(pathcopy1));
    strncpy(pathcopy2, fullpath, sizeof(pathcopy2));

    char *dir = dirname(pathcopy1);   // modifies pathcopy1
    char *base = basename(pathcopy2); // modifies pathcopy2

    // find last folder name inside dir
    char *lastslash = strrchr(dir, '/');

    if (lastslash != NULL && *(lastslash + 1) != '\0') {
        snprintf(result, sizeof(result), "%s/%s", lastslash, base);
    } else {
        snprintf(result, sizeof(result), "/%s", base);
    }

    return result;
}

void install_ipk() {
    char cmd[512] = "/usr/bin/opkg install --force-reinstall --force-overwrite \"install/";
    strcat(cmd,list_file[file_list_index]);
    strcat(cmd, "\"");

    FILE *fp;
    char buffer[256];
    char* before_list[256];
    char* after_list[256];
    u16 max_list_before = 0, max_list_after = 0;

    install_info[current_line] = strdup("Processing...");
    current_line++;
    update_video();

    scanning("/mnt/gmenu2x/sections", before_list, &max_list_before);

    // remount folder / as read write
    if (mount("/", "/", NULL, MS_REMOUNT, NULL) != 0)
        perror("remount as rw failed");

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

    // remount folder / as read only
    if (mount("/", "/", NULL, MS_REMOUNT | MS_RDONLY, NULL) != 0)
        perror("remount as ro failed");

    scanning("/mnt/gmenu2x/sections", after_list, &max_list_after);

    for (int i = 0; i < max_list_after; i++) {
        u8 found = 0;
        for (int j = 0; j < max_list_before; j++) {
            if (strcmp(after_list[i], before_list[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            char src[256], dst[256];
            snprintf(src, sizeof(src), "/mnt/gmenu2x/sections%s", short_path(after_list[i]));
            snprintf(dst, sizeof(dst), "/mnt/apps/mx/sections%s", short_path(after_list[i]));
            printf("%s\n",short_path(after_list[i]));
            copy_file(src, dst);
        }
    }
    done_massage = 1;
}

void uninstall_ipk() {
    char cmd[256];
    char package[128];
    char buffer[128];
    char* before_list[256];
    char* after_list[256];
    u16 max_list_before = 0, max_list_after = 0;

    snprintf(cmd, sizeof(cmd), "/usr/bin/opkg search \"*%s\" | cut -f1 -d' '",basename(package_name[link_index]));

    FILE *fp = popen(cmd,"r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strcpy(package, buffer);
        install_info[current_line] = strdup("Searching packages...");
        current_line++;
        update_video();
    }

    pclose(fp);

    if(package[0] == '\0') {
        printf("Package not found!");
        is_open_install = 0;
        return;
    }

    scanning("/mnt/gmenu2x/sections", before_list, &max_list_before);

    if (mount("/", "/", NULL, MS_REMOUNT, NULL) != 0)
        perror("remount as rw failed");

    snprintf(cmd, sizeof(cmd),"/usr/bin/opkg remove %s",package);
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
    if (mount("/", "/", NULL, MS_REMOUNT | MS_RDONLY, NULL) != 0)
        perror("remount as ro failed");

    scanning("/mnt/gmenu2x/sections", after_list, &max_list_after);

    for (int j = 0; j < max_list_before; j++) {
        int found = 0;
        for (int i = 0; i < max_list_after; i++) {
            if (strcmp(before_list[j], after_list[i]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            char package_removed[256];
            snprintf(package_removed, sizeof(package_removed), "/mnt/apps/mx/sections%s", short_path(before_list[j]));
            printf("%s\n",short_path(before_list[j]));
            remove(package_removed);
        }
    }

    done_massage = 1;
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

void save_rom_path() {
    char path[256];
    snprintf(path,sizeof(path),"sections/%s/%s",menu_title[section_index],list_name[link_index]);
    FILE* file = fopen(path,"r");
    if(!file) {
        printf("path not found");
    }
    char lines[256][256];
    int count = 0;
    while(fgets(lines[count],sizeof(lines[count]),file)) {
        if(strncmp(lines[count],"selectordir=", 12) == 0) {
            snprintf(lines[count],sizeof(lines[count]),"selectordir=%s\n",dir);
        }
        count++;
    }
    fclose(file);
    file = fopen(path,"w");
    for (int i = 0; i < count; i++) {
        fputs(lines[i], file);
    }
    fclose(file);
}

void run_command() {
    struct stat st;
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", dir, list_rom[rom_index]);
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        char newdir[256];
        snprintf(newdir, sizeof(newdir), "%s/%s", dir, list_rom[rom_index]);
        realpath(newdir, dir);
        update_bg = 1;
        clear_rom_list();
        load_rom_list();
    } else if(stat(path, &st) == 0 && S_ISREG(st.st_mode)){
        snprintf(command, sizeof(command), "%s \"%s/%s\"", command, dir, list_rom[rom_index]);
        save_rom_path();
        chdir(binary_path);
        close_font();
        close_video();
        clear_rom_list();
        clear_list_app();
        clear_file_list();
        done = 1;
        system(command);
    } else {
        chdir(binary_path);
        close_font();
        close_video();
        clear_rom_list();
        clear_list_app();
        clear_file_list();
        done = 1;
        system(command);
    }
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

#include "font.h"

s16 update_text_pos(char *filename, u16 index) {
    static s16 pos_x = 35;
    static u16 old_index = index;
#ifndef MIYOO
    static u8 frame = 0;
#endif
    // reset when index change
    if( old_index != index) {
        old_index = index;
        pos_x = 35;
#ifndef MIYOO
        frame = 0;
#endif
    }
    // check if legth text in pixel is long
    if(get_text_width(filename) >= 320 - 35) {
#ifndef MIYOO
        frame++;
        if(frame > 20) {
            pos_x -= 1;
            frame = 0;
        }
#else
        pos_x-=1;
#endif
        if(pos_x < 245 - get_text_width(filename))
            pos_x = 35;
    }
    return pos_x;
}

#include <sys/mman.h>

u32 get_cpu_mhz(u32 reg) {
    reg &= 0x0003FFFF;                   // discard stored MHz bits
    u32 n = ((reg >> 8) & 0x1F) + 1;     // Nf+1
    u32 k = ((reg >> 4) & 0x03) + 1;     // Kf+1
    u32 m = (reg & 0x03) + 1;            // Mf+1
    u32 p = 1 << ((reg >> 16) & 0x03);   // P
    u32 hz = 24000000UL * n * k / (m * p);
    return hz / 1000000;
}

void set_CPU(u8 values) {
    u32 oc_table[] {
    // F1C100S PLL_CPU Control Register.
    // 24MHz*N*K/(M*P) ; N = (Nf+1)<=32 ; K = (Kf+1)<=4 ; M = (Mf+1)<=4 ; P = (00: /1 | 01: /2 | 10: /4); --> CPU_PLL output must be in 200MHz~2.6GHz range
    // 27:18 are 10bit non-affecting space thus starting to read "int mhz" value here "(MHz << 18)" up to last 32bit.
    // ((24 * N * K) << 18) | (Nf << 8) | (Kf << 4) | (Mf << 0) | (Pf << 16),
    //
        (216 << 18) | (8 << 8) | (0 << 4),
        (408 << 18) | (16 << 8) | (0 << 4),
        (744 << 18) | (30 << 8) | (0 << 4),
    };
    volatile int memdev = open("/dev/mem", O_RDWR);
    if (memdev > 0) {
        u32 *mem = (u32*)mmap(0, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, memdev, 0x01c20000);
        if (mem == MAP_FAILED) {
            printf("Could not mmap hardware registers!");
            return;
        } else if(get_cpu_mhz(mem[0]) != 408) {
            // if cpu has been set, do nothing
        } else {
            mem[0] = (1 << 31) | (oc_table[values] & 0x0003ffff);
            //printf("New CPU clock: %u MHz\n",get_cpu_mhz(mem[0]));
        }
        munmap(mem, 0x1000);
    } else {
        printf("Could not open /dev/mem");
    }
    close(memdev);
}
