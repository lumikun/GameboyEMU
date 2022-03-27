#include "bus.h"
#include "cart.h"

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

u8 bus_read(u16 addr)
{
    if (addr < 0x8000) {
        return cart_read(addr);
    }
    printf("Err... Neatbalstts bus_read(%04X)\n", addr);
    //NO_IMPL
}

void bus_write(u16 addr, u8 val)
{
    if (addr < 0x8000) {
        cart_write(addr, val);
        return;
    }
    printf("Err... Neatbalstits bus_write(%04X)\n", addr);
    //NO_IMPL
}

u16 bus_read16(u16 addr)
{
    u16 lo = bus_read(addr);
    u16 hi = bus_read(addr + 1);

    return lo | (hi << 8);
}

void bus_write16(u16 addr, u16 val)
{
    bus_write(addr + 1, (val >> 8) & 0xFF);
    bus_write(addr, val & 0xFF);
}
