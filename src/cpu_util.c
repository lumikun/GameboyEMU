#include "cpu.h"

extern cpu_context ctx;

u16 reverse(u16 n)
{
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
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
