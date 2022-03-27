#include "cpu.h"
#include "bus.h"
#include "include.h"

extern cpu_context ctx;

u16 reverse(u16 n)
{
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c)
{
    if (z != -1)
        BIT_SET(ctx->reg.f, 7, z);
    if (n != -1)
        BIT_SET(ctx->reg.f, 6, n);
    if (h != -1)
        BIT_SET(ctx->reg.f, 5, h);
    if (c != -1)
        BIT_SET(ctx->reg.f, 4, c);
}

u16 cpu_read_reg(reg_type rt)
{
    switch(rt) {
        case RT_A:
            return ctx.reg.a;
        case RT_F:
            return ctx.reg.f;
        case RT_B:
            return ctx.reg.b;
        case RT_C:
            return ctx.reg.c;
        case RT_D:
            return ctx.reg.d;
        case RT_E:
            return ctx.reg.e;
        case RT_H:
            return ctx.reg.h;
        case RT_L:
            return ctx.reg.l;

        case RT_AF:
            return reverse(*((u16 *)&ctx.reg.a));
        case RT_BC:
            return reverse(*((u16 *)&ctx.reg.b));
        case RT_DE:
            return reverse(*((u16 *)&ctx.reg.d));
        case RT_HL:
            return reverse(*((u16 *)&ctx.reg.h));

        case RT_PC:
            return ctx.reg.pc;
        case RT_SP:
            return ctx.reg.sp;
        default:
            return 0;
    }
}

void cpu_set_reg(reg_type rt, u16 val)
{
    switch (rt) {
        case RT_A:
            ctx.reg.a = val & 0xFF;
            break;
        case RT_F:
            ctx.reg.f = val & 0xFF;
            break;
        case RT_B:
            ctx.reg.b = val & 0xFF;
            break;
        case RT_C:
            ctx.reg.c = val & 0xFF;
            break;
        case RT_D:
            ctx.reg.d = val & 0xFF;
            break;
        case RT_E:
            ctx.reg.e = val & 0xFF;
            break;
        case RT_H:
            ctx.reg.h = val & 0xFF;
            break;
        case RT_L:
            ctx.reg.l = val & 0xFF;
            break;

        case RT_AF:
            *((u16 *)&ctx.reg.a) = reverse(val);
            break;
        case RT_BC:
            *((u16 *)&ctx.reg.b) = reverse(val);
            break;
        case RT_DE:
            *((u16 *)&ctx.reg.d) = reverse(val);
            break;
        case RT_HL:
            *((u16 *)&ctx.reg.h) = reverse(val);
            break;

        case RT_PC:
            ctx.reg.pc = val;
            break;
        case RT_SP:
            ctx.reg.sp = val;
            break;
        case RT_NONE:
            break;
    }
}
void cpu_fetch_data()
{
    ctx.mem_dest = 0;
    ctx.dest_is_mem = false;

    if (ctx.cur_inst == NULL)
        return;

    switch (ctx.cur_inst->mode) {
        case AM_IMP: return;
        case AM_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_1);
            return;
        case AM_R_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            return;
        case AM_R_D8:
            ctx.fetched_data = bus_read(ctx.reg.pc);
            emu_cycles(1);
            ctx.reg.pc++;
            return;
        case AM_R_D16:
        case AM_D16: {
            u16 lo = bus_read(ctx.reg.pc);
            emu_cycles(1);
            u16 hi = bus_read(ctx.reg.pc+1);
            emu_cycles(1);
            ctx.fetched_data = lo | (hi << 8);
            ctx.reg.pc+=2;
            return;
        }
        case AM_MR_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;

            if (ctx.cur_inst->reg_1 == RT_C)
                ctx.mem_dest |= 0xFF00;
            return;
        case AM_R_MR: {
            u16 addr = cpu_read_reg(ctx.cur_inst->reg_2);
            if (ctx.cur_inst->reg_2 == RT_C)
                addr |= 0xFF00;
            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
        } return;
        case AM_R_HLI:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;
        case AM_R_HLD:
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;
        case AM_HLI_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;
        case AM_HLD_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem  = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;
        case AM_R_A8:
            ctx.fetched_data = bus_read(ctx.reg.pc);
            emu_cycles(1);
            ctx.reg.pc++;
            return;
        case AM_A8_R:
            ctx.mem_dest = bus_read(ctx.reg.pc) | 0xFF00;
            ctx.dest_is_mem = true;
            emu_cycles(1);
            ctx.reg.pc++;
            return;
        case AM_HL_SPR:
            ctx.fetched_data = bus_read(ctx.reg.pc);
            emu_cycles(1);
            ctx.reg.pc++;
            return;
        case AM_D8:
            ctx.fetched_data = bus_read(ctx.reg.pc);
            emu_cycles(1);
            ctx.reg.pc++;
            return;
        case AM_A16_R:
        case AM_D16_R: {
            u16 lo = bus_read(ctx.reg.pc);
            emu_cycles(1);
            u16 hi = bus_read(ctx.reg.pc+1);
            emu_cycles(1);
            ctx.mem_dest = lo | (hi << 8);
            ctx.dest_is_mem = true;
            ctx.reg.pc += 2;
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
        } return;
        case AM_MR_D8:
            ctx.fetched_data = bus_read(ctx.reg.pc);
            emu_cycles(1);
            ctx.reg.pc++;
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            return;
        case AM_MR:
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_1));
            emu_cycles(1);
            return;
        case AM_R_A16: {
            u16 lo = bus_read(ctx.reg.pc);
            emu_cycles(1);
            u16 hi = bus_read(ctx.reg.pc+1);
            emu_cycles(1);
            u16 addr = lo | (hi << 8);
            ctx.reg.pc+=2;
            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
            return;
        }
        default:
            printf("Err... Unknown Addressing Mode! %d (%02X)\n", ctx.cur_inst->mode, ctx.cur_opcode);
            exit(-7);
            return;
    }
}
