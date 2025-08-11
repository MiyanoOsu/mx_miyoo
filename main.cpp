#include "common.h"
#include "video.h"
#include "input.h"
#include "config.h"
#include "font.h"
#include "mx.h"

u8 done = 0;

int main() {
    init_config();
    init_font();
    init_video();
    load_section();
#ifdef MIYOO
    set_CPU(1);
#endif
    while(!done) {
        handle_input();
        load_battery();
        update_video();
    }
    close_font();
    close_video();
    return 0;
}