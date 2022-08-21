#include "ppu.h"
#include "lcd.h"
#include "cpu.h"
#include "interrupt.h"
#include "util.h"
#include <string.h>

static u32 target_frame_time = 1000/60;
static long prev_frame_time = 0;
static long start_timer = 0;
static long frame_count = 0;

void pipeline_fifo_rest();
void pipeline_process();
bool window_visible();

void increment_ly()
{
    if (window_visible() && lcd_get_ctx()->ly >= lcd_get_ctx()->win_y &&
        lcd_get_ctx()->ly < lcd_get_ctx()->win_y + YRES) {
            ppu_get_ctx()->window_line++;
    }

    lcd_get_ctx()->ly++;
    if (lcd_get_ctx()->ly == lcd_get_ctx()->ly_compare) {
        LCDS_LYC_SET(1);
        if (LCDS_STAT_INT(SS_LYC)) {
            cpu_req_inter(IT_LCD_STAT);
        }
    } else {
        LCDS_LYC_SET(0);
    }
}

void load_line_sprites()
{
    int cur_y = lcd_get_ctx()->ly;
    u8 sprite_height = LCDC_OBJ_HEIGHT;
    memset(ppu_get_ctx()->line_entry_arr, 0, sizeof(ppu_get_ctx()->line_entry_arr));
    for (int i = 0; i < 40; i++) {
        oam_entry e = ppu_get_ctx()->oam_ram[i];
        if (!e.x)
            continue;
        if (ppu_get_ctx()->line_sprite_count >= 10)
            break;
        if (e.y <= cur_y + 16 && e.y + sprite_height > cur_y + 16) {
            oam_line_entry *entry = &ppu_get_ctx()->line_entry_arr[ppu_get_ctx()->line_sprite_count++];
            entry->entry = e;
            entry->next = NULL;
            if (!ppu_get_ctx()->line_sprites || ppu_get_ctx()->line_sprites->entry.x > e.x) {
                entry->next = ppu_get_ctx()->line_sprites;
                ppu_get_ctx()->line_sprites = entry;
                continue;
            }
            oam_line_entry *le = ppu_get_ctx()->line_sprites;
            oam_line_entry *prev = le;
            while (le) {
                if (le->entry.x > e.x) {
                    prev->next = entry;
                    entry->next = le;
                    break;
                }
                if (!le->next) {
                    le->next = entry;
                    break;
                }
                prev = le;
                le = le->next;
            }
        }
    }
}


void ppu_mode_oam()
{
    if (ppu_get_ctx()->line_tick >= 80) {
        LCDS_MODE_SET(MODE_XFER);

        ppu_get_ctx()->pfc.cur_fetch_state = FS_TILE;
        ppu_get_ctx()->pfc.line_x = 0;
        ppu_get_ctx()->pfc.fetch_x = 0;
        ppu_get_ctx()->pfc.pushed_x = 0;
        ppu_get_ctx()->pfc.fifo_x = 0;
    }
    if (ppu_get_ctx()->line_tick == 1) {
        ppu_get_ctx()->line_sprites = 0;
        ppu_get_ctx()->line_sprite_count = 0;
        load_line_sprites();
    }
}

void ppu_mode_xfer()
{
    pipeline_process();
    if (ppu_get_ctx()->pfc.pushed_x >= XRES) {
        pipeline_fifo_rest();
        LCDS_MODE_SET(MODE_HBLANK);
        if (LCDS_STAT_INT(SS_HBLANK))
            cpu_req_inter(IT_LCD_STAT);
    }
}

void ppu_mode_vblank()
{
    if (ppu_get_ctx()->line_tick >= TICKS_PER_LINE) {
        increment_ly();
        if (lcd_get_ctx()->ly >= LINES_PER_FRAME) {
            LCDS_MODE_SET(MODE_OAM);
            lcd_get_ctx()->ly = 0;
            ppu_get_ctx()->window_line = 0;
        }
        ppu_get_ctx()->line_tick = 0;
    }
}

void ppu_mode_hblank()
{
    if (ppu_get_ctx()->line_tick >= TICKS_PER_LINE) {
        increment_ly();
        if (lcd_get_ctx()->ly >= YRES) {
            LCDS_MODE_SET(MODE_VBLANK);
            cpu_req_inter(IT_VBLANK);
            if (LCDS_STAT_INT(SS_VBLANK))
                cpu_req_inter(IT_LCD_STAT);
            ppu_get_ctx()->current_frame++;
            u32 end = get_ticks();
            u32 frame_time = end - prev_frame_time;

            if (frame_time < target_frame_time)
                delay((target_frame_time - frame_time));

            if (end - start_timer >= 1000) {
                u32 fps = frame_count;
                start_timer = end;
                frame_count = 0;
                printf("FPS: %d\n", fps);
            }
            frame_count++;
            prev_frame_time = get_ticks();
        } else {
            LCDS_MODE_SET(MODE_OAM);
        }
        ppu_get_ctx()->line_tick = 0;
    }
}
