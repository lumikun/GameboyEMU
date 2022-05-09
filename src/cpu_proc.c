#include "cpu.h"
#include "util.h"
#include "include.h"
#include "bus.h"

reg_type rt_lookup[] = {
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_HL,
    RT_A
};

reg_type decode_reg(u8 reg)
{
    if (reg > 0b111) {
        return RT_NONE;
    }
    return rt_lookup[reg];
}

static void proc_none(cpu_context *ctx)
{
    printf("Nepareiza instrukcija!\n");
    exit(-7);
}

static void proc_nop(cpu_context *ctx)
{}

static void proc_di(cpu_context *ctx)
{
    ctx->int_master_enabled = false;
}

statoc void proc_cb(cpu_context *ctx) 
{
    u8 op = ctx->fetched_data;
    reg_type reg = decode_reg(op & 0b111);
    u8 bit = (op >> 3) & 0b111;
    u8 bit_op = (op >> 6) & 0b11;
    u8 reg_val = cpu_read_reg8(reg);

    emu_cycles(1);

    if (reg == RT_HL) {
        emu_cycles(2);
    }

    switch(bit_op) {
        case 1:
            cpu_set_flags(ctx, !(reg_val & (1 << bit)), 0, 1, -1);
            return;
        case 2:
            reg_val &= ~(1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;
        case 3:
            reg_val |= (1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;
    }

    bool flagC = CPU_FLAG_C;

    switch(bit) {
        case 0: {
            bool setC = false;
            u8 result = (reg_val << 1) & 0xFF;
            if ((reg_val & (1 << 7)) != 0) {
                result |= 1;
                setC = true;
            }

            cpu_set_reg8(reg, result);
            cpu_set_flags(ctx, result == 0, false, false, setC);
        } return;
        case 1: {
            u8 old = reg_val;
            reg_val >>= 1;
            reg_val |= (old << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, old & 1);
        } return;
        case 2: {
            u8 old = reg_val;
            reg_val <<= 1;
            reg_val |= flagC;

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
        } return;
        case 3: {
            u8 old = reg_val;
            reg_val >>= 1;

            reg_val |= (flagC << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, old & 1);
        } return;
        case 4: {
            u8 old = reg_val;
            reg_val <<= 1;

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
        } return;
        case 5: {
            u8 u = (int8_t)reg_val >> 1;
            cpu_set_reg8(reg, u);
            cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
        } return;
        case 6: {
            reg_val = ((reg_val & 0xF0) >> 4) | ((reg_val & 0xF) << 4);
            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(ctx, reg_val == 0, false, false, false);
        } return;
        case 7: {
            u8 u = reg_val >> 1;
            cpu_set_reg8(reg, u);
            cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
        } return;
    }

    fprintf(stderr, "Err... INVALID CB: %02X\n", op);
    NO_IMPL 
}

static void proc_rlca(cpu_context *ctx) 
{
    u8 b = ctx->reg.a & 1;
    ctx->reg.a >>= 1;
    ctx->reg.a |= (b << 7);
    cpu_set_flags(ctx, 0, 0, 0, b);
}

static void proc_rrca(cpu_context *ctx) 
{
    u8 b = ctx->reg.a & 1;
    ctx->reg.a >>= 1;
    ctx->reg.a |= (b << 7);
    cpu_set_flags(ctx, 0, 0, 0, b);
}

static void proc_rla(cpu_context *ctx) 
{
    u8 u = ctx->reg.a;
    u8 cf = CPU_FLAG_C;
    u8 c = (u >> 7) & 1;
    ctx->reg.a = (u << 1) | cf;
    cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_stop(cpu_context *ctx) 
{
    fprintf(stderr, "Err... STOPPING!\n");
    NO_IMPL
}

static void proc_daa(cpu_context *ctx) 
{
    u8 u = 0;
    int fc = 0;
    if (CPU_FLAG_H || (!CPU_FLAG_N && (ctx->reg.a & 0xF) > 9)) 
        u = 6;
    if (CPU_FLAG_C || (!CPU_FLAG_N && ctx->reg.a > 0x99)) {
        u |= 0x60;
        fc = 1;
    }
    ctx->reg.a += CPU_FLAG_N ? -u : u;
    cpu_set_flags(ctx, ctx->reg.a == 0, -1, 0, fc);
}

static void proc_cp(cpu_context *ctx) 
{
    int n = (int)ctx->reg.a - (int)ctx->fetched_data;
    cpu_set_flags(ctx, n == 0, 1, 
        ((int)ctx->reg.a & 0x0F) - ((int)ctx->fetched_data & 0x0F) < 0, n < 0);
}

static void proc_cpl(cpu_context *ctx) 
{
    ctx->reg.a = ~ctx->reg.a;
    cpu_set_flags(ctx, -1, 1, 1, -1);
}

static void proc_scf(cpu_context *ctx) 
{
    cpu_set_flags(ctx, -1, 0, 0, 1);
}

static void proc_ccf(cpu_context *ctx) 
{
    cpu_set_flags(ctx, -1, 0, 0, CPU_FLAG_C ^ 1);
}

static void proc_halt(cpu_context *ctx) 
{
    ctx->halted = true;
}

static void proc_rra(cpu_context *ctx) 
{
    u8 carry = CPU_FLAG_C;
    u8 new_c = ctx->reg.a & 1;
    ctx->reg.a >>= 1;
    ctx->reg.a |= (carry << 7);
    cpu_set_flags(ctx, 0, 0, 0, new_c);
}

static void proc_and(cpu_context *ctx) 
{
    ctx->reg.a &= ctx->fetched_data;
    cpu_set_flags(ctx, ctx->reg.a == 0, 0, 1, 0);
}

// LD - Load instrukcija.
static void proc_ld(cpu_context *ctx)
{
    if (ctx->dest_is_mem) {
        if (ctx->cur_inst->reg_2 >= RT_AF) {
            emu_cycles(1);
            bus_write16(ctx->mem_dest, ctx->fetched_data);
        } else {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }
        return;
    }
    if (ctx->cur_inst->mode == AM_HL_SPR) {
        u8 hflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        u8 cflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_2) + (char)ctx->fetched_data);
        return;
    }
    cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);
}

static void proc_ldh(cpu_context *ctx)
{
    if (ctx->cur_inst->reg_1 == RT_A) {
        cpu_set_reg(ctx->cur_inst->reg_1, bus_read(0xFF00 | ctx->fetched_data));
    } else {
        bus_write(ctx->mem_dest, ctx->reg.a);
    }
    emu_cycle(1);
}

static void proc_di(cpu_context *ctx) 
{
    ctx->int_master_enabled = false;
}

static void proc_ei(cpu_context *ctx) 
{
    ctx->enabling_ime = true;
}

static bool is_16_bit(reg_type rt) 
{
    return rt >= RT_AF;
}

static void proc_or(cpu_context *ctx) 
{
    ctx->reg.a |= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->reg.a == 0, 0, 0, 0);
}

static void proc_xor(cpu_context *ctx)
{
    ctx->reg.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->reg.a == 0, 0, 0, 0);
}

static bool proc_check_cond(cpu_context *ctx)
{
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch(ctx->cur_inst->cond) {
        case CT_NONE:
            return true;
        case CT_C:
            return c;
        case CT_NC:
            return !c;
        case CT_Z:
            return z;
        case CT_NZ:
            return !z;
    }
    return false;
}

// move to cpu_util.h
static void goto_addr(cpu_context *ctx, u16 addr, bool pushpc)
{
    if (proc_check_cond(ctx)) {
        if (pushpc) {
            emu_cycles(2);
            stack_push16(ctx->reg.pc);
        }
        ctx->reg.pc = addr;
        emu_cycles(1);
    }
}

static void proc_jp(cpu_context *ctx)
{
    if (proc_check_cond(ctx)) {
        ctx->reg.pc = ctx->fetched_data;
        emu_cycles(1);
    }
}

static void proc_jr(cpu_context *ctx)
{
    char rel = (char)(ctx->fetched_data & 0xFF);
    u16 addr = ctx->reg.pc + rel;
    goto_addr(ctx, addr, false);
}

static void proc_call(cpu_context *ctx)
{
    goto_addr(ctx, ctx->fetched_data, true);
}

static void proc_rst(cpu_context *ctx)
{
    goto_addr(ctx, ctx->cur_inst->param, true);
}

static void proc_ret(cpu_context *ctx)
{
    if (ctx->cur_inst->cond != CT_NONE)
        emu_cycles(1);
    if (proc_check_cond(ctx)) {
        u16 lo = stack_pop();
        emu_cycle(1);
        u16 hi = stack_pop();
        emu_cycle(1);

        u16 n = (hi << 8) | lo;
        ctx->reg.pc = n;

        emu_cycles(1);
    }
}

static void proc_reti(cpu_context *ctx)
{
    ctx->int_master_enabled = true;
    proc_ret(ctx);
}

static void proc_pop(cpu_context *ctx)
{
    u16 lo = stack_pop();
    emu_cycles(1);
    u16 hi = stack_pop();
    emu_cycles(1);

    u16 n = (hi << 8) | lo;
    cpu_set_reg(ctx->cur_inst->reg_1, n);
    if (ctx->cur_inst->reg_1 == RT_AF)
        cpu_set_reg(ctx->cur_inst->reg_1, n & 0xFFF0);
}

static void proc_push(cpu_context *ctx)
{
    u16 hi = (cpu_read_reg(ctx->cur_inst->reg_1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi);

    u16 lo = cpu_read_reg(ctx->cur_inst->reg_1) & 0xFF;
    emu_cycles(1);
    stack_push(lo);

    emu_cycles(1);
}

static void proc_sub(cpu_context *ctx) 
{
    u16 val = cpu_read_reg(ctx->cur_inst->reg_1) - ctx->fetched_data;
    int z = val == 0;
    int h = ((int)cpu_read_reg(ctx->cur_inst->reg_1) & 0xF) - ((int)ctx->fetched_data & 0xF) < 0;
    int c = ((int)cpu_read_reg(ctx->cur_inst->reg_1)) - ((int)ctx->fetched_data) < 0;
    cpu_set_reg(ctx->cur_inst->reg_1, val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_sbc(cpu_context *ctx) 
{
    u8 val = ctx->fetched_data + CPU_FLAG_C;
    int z = cpu_read_reg(ctx->cur_inst->reg_1) - val == 0;
    int h = ((int)cpu_read_reg(ctx->cur_inst->reg_1) & 0xF) 
        - ((int)ctx->fetched_data & 0xF) - ((int)CPU_FLAG_C) < 0;
    int c = ((int)cpu_read_reg(ctx->cur_inst->reg_1)) 
        - ((int)ctx->fetched_data) - ((int)CPU_FLAG_C) < 0;
    cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_1) - val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_adc(cpu_context *ctx) 
{
    u16 u = ctx->fetched_data;
    u16 a = ctx->reg.a;
    u16 c = CPU_FLAG_C;
    ctx->reg.a = (a + u + c) & 0xFF;
    cpu_set_flags(ctx, ctx->reg.a == 0, 0, 
        (a & 0xF) + (u & 0xF) + c > 0xF,
        a + u + c > 0xFF);
}

static void proc_add(cpu_context *ctx) 
{
    u32 val = cpu_read_reg(ctx->cur_inst->reg_1) + ctx->fetched_data;
    bool is_16bit = is_16_bit(ctx->cur_inst->reg_1);
    if (is_16bit) 
        emu_cycles(1);
    if (ctx->cur_inst->reg_1 == RT_SP) 
        val = cpu_read_reg(ctx->cur_inst->reg_1) + (char)ctx->fetched_data;
    
    int z = (val & 0xFF) == 0;
    int h = (cpu_read_reg(ctx->cur_inst->reg_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
    int c = (int)(cpu_read_reg(ctx->cur_inst->reg_1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) >= 0x100;

    if (is_16bit) {
        z = -1;
        h = (cpu_read_reg(ctx->cur_inst->reg_1) & 0xFFF) + (ctx->fetched_data & 0xFFF) >= 0x1000;
        u32 n = ((u32)cpu_read_reg(ctx->cur_inst->reg_1)) + ((u32)ctx->fetched_data);
        c = n >= 0x10000;
    }
    if (ctx->cur_inst->reg_1 == RT_SP) {
        z = 0;
        h = (cpu_read_reg(ctx->cur_inst->reg_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        c = (int)(cpu_read_reg(ctx->cur_inst->reg_1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) > 0x100;
    }

    cpu_set_reg(ctx->cur_inst->reg_1, val & 0xFFFF);
    cpu_set_flags(ctx, z, 0, h, c);
}


static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_LDH] = proc_ldh,
    [IN_JP] = proc_jp,
    [IN_DI] = proc_di,
    [IN_POP] = proc_pop,
    [IN_PUSH] = proc_push,
    [IN_JR] = proc_jr,
    [IN_CALL] = proc_call,
    [IN_RET] = proc_ret,
    [IN_RST] = proc_rst,
    [IN_DEC] = proc_dec,
    [IN_INC] = proc_inc,
    [IN_ADD] = proc_add,
    [IN_ADC] = proc_adc,
    [IN_SUB] = proc_sub,
    [IN_SBC] = proc_sbc,
    [IN_AND] = proc_and,
    [IN_XOR] = proc_xor,
    [IN_OR] = proc_or,
    [IN_CP] = proc_cp,
    [IN_CB] = proc_cb,
    [IN_RRCA] = proc_rrca,
    [IN_RLCA] = proc_rlca,
    [IN_RRA] = proc_rra,
    [IN_RLA] = proc_rla,
    [IN_STOP] = proc_stop,
    [IN_HALT] = proc_halt,
    [IN_DAA] = proc_daa,
    [IN_CPL] = proc_cpl,
    [IN_SCF] = proc_scf,
    [IN_CCF] = proc_ccf,
    [IN_EI] = proc_ei,
    [IN_RETI] = proc_reti
};

IN_PROC inst_get_processor(in_type type)
{
    return processors[type];
}
