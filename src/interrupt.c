#include "cpu.h"
#include "stack.h"
#include "interrupt.h"

void inter_handle(cpu_context *ctx, u16 addr) 
{
    stack_push16(ctx->reg.pc);
    ctx->reg.pc = addr;
}

bool inter_check(cpu_context *ctx, u16 addr, inter_type it) 
{
    if (ctx->inter_flag & it && ctx->ie_register & it) {
        inter_handle(ctx, addr);
        ctx->inter_flag &= ~it;
        ctx->halted = false;
        ctx->int_master_enabled = false;
        return true;
    }
    return false;
}
