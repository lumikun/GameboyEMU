#include "cpu.h"
#include "bus.h"
#include "interrupt.h"
#include "dbg.h"
#include "timer.h"
#include "include.h"

cpu_context ctx = {0};

/* Procesora Uzsaksanas funkcija. */

void cpu_init()
{
    ctx.reg.pc = 0x100;
    ctx.reg.sp = 0xFFFF;
    *((short *)&ctx.reg.a) = 0xB001;
    *((short *)&ctx.reg.b) = 0x1300;
    *((short *)&ctx.reg.d) = 0xD800;
    *((short *)&ctx.reg.h) = 0x4D01;
    ctx.ie_register = 0;
    ctx.inter_flag = 0;
    ctx.int_master_enabled = false;
    ctx.ime_enable = false;

    timer_get_context()->div = 0xABCC;
}

static void cpu_fetch_inst()
{
    ctx.cur_opcode = bus_read(ctx.reg.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
}

static void cpu_execute()
{
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

    if (!proc) {
        NO_IMPL
    }
    proc(&ctx);
}

bool cpu_step()
{
    if (!ctx.halted) {
        u16 pc = ctx.reg.pc;

        cpu_fetch_inst();
        emu_cycles(1);
        cpu_fetch_data();
    // cleanup TODO!
        char flags[16];
        sprintf(flags, "%c%c%c%c", 
            ctx.reg.f & (1 << 7) ? 'Z' : '-',
            ctx.reg.f & (1 << 6) ? 'N' : '-',
            ctx.reg.f & (1 << 5) ? 'H' : '-',
            ctx.reg.f & (1 << 4) ? 'C' : '-'
        );

        char inst[16];
        inst_to_str(&ctx, inst);

        printf("%08lX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n", 
            emu_get_context()->ticks,
            pc, inst, ctx.cur_opcode,
            bus_read(pc + 1), bus_read(pc + 2), ctx.reg.a, flags, ctx.reg.b, ctx.reg.c,
            ctx.reg.d, ctx.reg.e, ctx.reg.h, ctx.reg.l);

        if (ctx.cur_inst == NULL) {
            printf("Err...Unknown Instruction! %02X\n", ctx.cur_opcode);
            exit(-7);
        }

        dbg_update();
        dbg_print();

        cpu_execute();
    } else {
        //is halted...
        emu_cycles(1);

        if (ctx.inter_flag) {
            ctx.halted = false;
        }
    }

    if (ctx.int_master_enabled) {
        cpu_handle_inter(&ctx);
        ctx.ime_enable = false;
    }

    if (ctx.ime_enable) {
        ctx.int_master_enabled = true;
    }

    return true;
}

void cpu_handle_inter(cpu_context *ctx) 
{
    if (inter_check(ctx, 0x40, IT_VBLANK)) {

    } else if (inter_check(ctx, 0x48, IT_LCD_STAT)) {
        
    } else if (inter_check(ctx, 0x50, IT_TIMER)) {

    } else if (inter_check(ctx, 0x58, IT_SERIAL)) {

    } else if (inter_check(ctx, 0x60, IT_JOYPAD)) {
        
    }
}

u8 cpu_get_ie_register() {
    return ctx.ie_register;
}

void cpu_set_ie_register(u8 n) {
    ctx.ie_register = n;
}

void cpu_req_inter(inter_type t)
{
    ctx.inter_flag |= t;
}
