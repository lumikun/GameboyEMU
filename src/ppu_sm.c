#include "ppu.h"
#include "lcd.h"
#include "cpu.h"
#include "interrupt.h"
#include "util.h"

static u32 target_frame_time = 1000/60;
static long prev_frame_time = 0;
static long start_timer = 0;
static long frame_count = 0;


void increment_ly()
{
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

void ppu_mode_oam()
{
    if (ppu_get_ctx()->line_tick >= 80)
        LCDS_MODE_SET(MODE_XFER);
}

void ppu_mode_xfer()
{
    if (ppu_get_ctx()->line_tick >= 80 + 172)
        LCDS_MODE_SET(MODE_HBLANK);
}

void ppu_mode_vblank()
{
    if (ppu_get_ctx()->line_tick >= TICKS_PER_LINE) {
        increment_ly();
        if (lcd_get_ctx()->ly >= LINES_PER_FRAME) {
            LCDS_MODE_SET(MODE_OAM);
            lcd_get_ctx()->ly = 0;
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
