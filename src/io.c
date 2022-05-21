#include "io.h"
#include "dma.h"


static char serial_data[2];

u8 ly = 0;

u8 io_read(u16 addr) 
{
    if (addr == 0xFF01) {
        return serial_data[0];
    }

    if (addr == 0xFF02) {
        return serial_data[1];
    }

    if (BETWEEN(addr, 0xFF04, 0xFF07)) {
        return timer_read(addr);
    }

    if (addr == 0xFF0F) {
        return cpu_get_inter_flag();
    }

    if (addr == 0xFF44)
        return ly++;


    printf("Err... UNSUPPORTED bus_read(%04X)\n", addr);
    return 0;
}

void io_write(u16 addr, u8 val)
{
    if (addr == 0xFF01) {
        serial_data[0] = val;
        return;
    }

    if (addr == 0xFF02) {
        serial_data[1] = val;
        return;
    }

    if (BETWEEN(addr, 0xFF04, 0xFF07)) {
        timer_write(addr, val);
        return;
    }
    
    if (addr == 0xFF0F) {
        cpu_set_inter_flag(val);
        return;
    }

    if (addr == 0xFF46) {
        dma_start(val);
        printf("DMA START!\n");
    }

    printf("Err... UNSUPPORTED bus_write(%04X)\n", addr);
}
