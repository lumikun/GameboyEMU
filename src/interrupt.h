#pragma once

#include "cpu.h"


typedef enum {
    IT_VBLANK = 1,
    IT_LCD_STAT = 2,
    IT_TIMER = 4,
    IT_SERIAL = 8,
    IT_JOYPAD = 16
} inter_type;

void inter_handle(cpu_context *ctx, u16 addr);
bool inter_check(cpu_context *ctx, u16 addr, inter_type it);

void cpu_handle_inter(cpu_context *ctx);
void cpu_req_inter(inter_type it);
