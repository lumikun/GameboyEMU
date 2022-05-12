#pragma once

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"
#include "cart.h"
#include "cpu.h"
#include "ui.h"
#include "timer.h"


typedef struct {
    bool paused;
    bool running;
    bool die;
    u64 ticks;
} emu_context;

void delay(u32 ms);
emu_context *emu_get_context();
void emu_cycles(int cpu_cycles);
void *cpu_run(void *p);
