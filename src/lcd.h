#pragma once

#include "include.h"

typedef struct {
    u8 lcdc;
    u8 lcds;
    u8 scroll_y;
    u8 scroll_x;
    u8 ly;
    u8 ly_compare;
    u8 dma;
    u8 bg_palette;
    u8 obj_palette[2];
    u8 win_y;
    u8 win_x;
    u32 bg_colors[4];
    u32 sp1_colors[4];
    u32 sp2_colors[4];
} lcd_context;

typedef enum {
    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM,
    MODE_XFER
} lcd_mode;

typedef enum {
    SS_HBLANK = (1 << 3),
    SS_VBLANK = (1 << 4),
    SS_OAM = (1 << 5),
    SS_LYC = (1 << 6),
} stat_src;

lcd_context *lcd_get_ctx();

#define LCDC_BGW_ENABLE (BIT(lcd_get_ctx()->lcdc, 0))
#define LCDC_OBJ_ENABLE (BIT(lcd_get_ctx()->lcdc, 1))
#define LCDC_OBJ_HEIGHT (BIT(lcd_get_ctx()->lcdc, 2) ? 16 : 8)
#define LCDC_BG_MAP_AREA (BIT(lcd_get_ctx()->lcdc, 3) ? 0x9C00 : 0x9800)
#define LCDC_BGW_DATA_AREA (BIT(lcd_get_ctx()->lcdc, 4) ? 0x8000 : 0x8800)
#define LCDC_WIN_ENABLE (BIT(lcd_get_ctx()->lcdc, 5))
#define LCDC_WIN_MAP_AREA (BIT(lcd_get_ctx()->lcdc, 6) ? 0x9C00 : 0x9800)
#define LCDC_LCD_ENABLE (BIT(lcd_get_ctx()->lcdc, 7))

#define LCDS_MODE ((lcd_mode)(lcd_get_ctx()->lcds & 0b11))
#define LCDS_MODE_SET(mode) { lcd_get_ctx()->lcds &= ~0b11; lcd_get_ctx()->lcds |= mode;}
#define LCDS_LYC (BIT(lcd_get_ctx()->lcds, 2))
#define LCDS_LYC_SET(b) (BIT_SET(lcd_get_ctx()->lcds, 2, b))
#define LCDS_STAT_INT(src) (lcd_get_ctx()->lcds & src)

void lcd_init();

u8 lcd_read(u16 addr);
void lcd_write(u16 addr, u8 val);

