#include "dma.h"
#include "ppu.h"
#include "bus.h"

typedef struct {
    bool active;
    u8 byte;
    u8 val;
    u8 start_delay;
} dma_context;

static dma_context ctx;

void dma_start(u8 start)
{
    ctx.active = true;
    ctx.byte = 0;
    ctx.start_delay = 2;
    ctx.val = start;
}

void dma_tick()
{
    if (!ctx.active)
        return;
    if (ctx.start_delay) {
        ctx.start_delay--;
        return;
    }
    ppu_oam_write(ctx.byte, bus_read((ctx.val * 0x100) + ctx.byte));
    ctx.byte++;
    ctx.active = ctx.byte < 0xA0;
    if (!ctx.active) {
        printf("DMA FINISHED!\n");
        sleep(2);
    }
}

bool dma_transfering()
{
    return ctx.active;
}
