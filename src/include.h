#pragma once

#include "cart.h"

typedef struct {
    bool paused;
    bool running;
    u64 ticks;
} emu_context;

void delay(u32 ms);
emu_context *emu_get_context();