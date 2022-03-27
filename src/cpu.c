#include "cpu.h"
#include "bus.h"
#include "include.h"

cpu_context ctx = {0};

/* Procesora Uzsaksanas funkcija. */

void cpu_init()
{
    ctx.reg.pc = 0x100;
    ctx.reg.a = 0x01;
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
        cpu_fetch_data();

        printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
               pc, inst_name(ctx.cur_inst->type), ctx.cur_opcode, bus_read(pc + 1), bus_read(pc + 2), ctx.reg.a, ctx.reg.b, ctx.reg.c);

        if (ctx.cur_inst == NULL) {
            printf("Err... Nezinama Instrukcija! %02X\n", ctx.cur_opcode);
            exit(-7);
        }

        cpu_execute();
    }
    return true;
}
