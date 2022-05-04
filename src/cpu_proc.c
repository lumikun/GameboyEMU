#include "cpu.h"
#include "util.h"
#include "include.h"
#include "bus.h"

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
[IN_RETI] = proc_reti,
[IN_XOR] = proc_xor

};

IN_PROC inst_get_processor(in_type type)
{
    return processors[type];
}
