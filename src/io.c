#include "io.h"
#include "dma.h"
#include "lcd.h"
#include "timer.h"
#include "cpu.h"
#include "input.h"

static char serial_data[2];
u8 ly = 0;

u8 io_read(u16 addr) 
{
    if (addr == 0xFF00) {
        return input_get_output();
    }

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

    if (BETWEEN(addr, 0xFF10, 0xFF3F)) {
        return 0;
    }

    if (BETWEEN(addr, 0xFF40, 0xFF4B)) {
        return lcd_read(addr);
    }

    printf("Err... UNSUPPORTED bus_read(%04X)\n", addr);
    return 0;
}

// BUG somewhere here !!!

void io_write(u16 addr, u8 val)
{
    if (addr == 0xFF00){
        input_set_sel(val);
        return;
    }

    if (addr == 0xFF01) {
        serial_data[0] = val;
        return;
    }

    if (addr == 0xFF02) {
        serial_data[1] = val;
        return;
    }
    //if (addr == 0xFF46) {
    //    dma_start(val);
    //    printf("DMA_START\n");
    //}

    if (BETWEEN(addr, 0xFF04, 0xFF07)) {
        timer_write(addr, val);
        return;
    }
    
    if (addr == 0xFF0F) {
        cpu_set_inter_flag(val);
        return;
    }

    if (BETWEEN(addr, 0xFF10, 0xFF3F)) {
        return;
    }

    if (BETWEEN(addr, 0xFF40, 0xFF4B)){
        lcd_write(addr, val);
        return;
    }

    printf("Err... UNSUPPORTED from IO WRITE bus_write(%04X)\n", addr);
}
