#pragma once

#include "util.h"
#include "instr.h"

typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
} cpu_reg;

typedef struct {
    cpu_reg reg;
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction *cur_inst;

    bool halted;
    bool stepping;
    bool int_master_enabled;
} cpu_context;


void cpu_init();
bool cpu_step();

typedef void (*IN_PROC)(cpu_context *);

IN_PROC inst_get_processor(in_type type);

#define CPU_FLAG_Z BIT(ctx->reg.f, 7)
#define CPU_FLAG_C BIT(ctx->reg.f, 4)

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c);
u16 cpu_read_reg(reg_type rt);
void cpu_set_reg(reg_type rt, u16 val);
void cpu_fetch_data();