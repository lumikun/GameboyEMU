#pragma once

#include <stdio.h>
#include "util.h"
#include "cart.h"
#include "cpu.h"


typedef struct {
    bool paused;
    bool running;
    u64 ticks;
} emu_context;

void delay(u32 ms);
emu_context *emu_get_context();
void emu_cycles(int cpu_cycles);
