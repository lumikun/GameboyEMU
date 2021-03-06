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
    bool ime_enable;

    u8 ie_register;
    u8 inter_flag;

} cpu_context;

cpu_reg *cpu_get_reg();

void cpu_init();
bool cpu_step();

typedef void (*IN_PROC)(cpu_context *);

IN_PROC inst_get_processor(in_type type);

#define CPU_FLAG_Z BIT(ctx->reg.f, 7) // Zero flag
#define CPU_FLAG_N BIT(ctx->reg.f, 6) // Subtraction flag
#define CPU_FLAG_H BIT(ctx->reg.f, 5) // Half-carry flag
#define CPU_FLAG_C BIT(ctx->reg.f, 4) // Carry flag

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c);
u16 cpu_read_reg(reg_type rt);
void cpu_set_reg(reg_type rt, u16 val);
u8 cpu_read_reg8(reg_type rt);
void cpu_set_reg8(reg_type rt, u8 val);
void cpu_fetch_data();

u8 cpu_get_inter_flag();
void cpu_set_inter_flag(u8 val);

u8 cpu_get_ie_register();
void cpu_set_ie_register(u8 n);

void inst_to_str(cpu_context *ctx, char *str);