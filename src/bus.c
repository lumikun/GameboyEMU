#include "bus.h"
#include "cart.h"
#include "ram.h"
#include "cpu.h"

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
    } else if (addr < 0xA000) {
        printf("Err... UNSUPPORTED bus_read(%04X)\n", addr);
        NO_IMPL
    } else if (addr < 0xC000) {
        return cart_read(addr);
    } else if (addr < 0xE000) {
        return wram_read(addr);  // Working RAM
    } else if (addr < 0xFE00) {
        return 0;                // Reserved Echo RAM
    } else if (addr < 0xFEA0) {
        printf("Err... UNSUPPORTED bus_read(%04X)\n", addr);
        NO_IMPL                  // OAM TODO!!
    } else if (addr < 0xFF00) {
        return 0;                // Reserved Unusable
    } else if (addr < 0xFF80) {
        printf("Err... UNSUPPORTED bus_read(%04X)\n", addr);
        NO_IMPL
    } else if (addr == 0xFFFF) {
        return cpu_get_ie_register(); // CPU enable register & TODO!!!
    }
    return hram_read(addr);
}

void bus_write(u16 addr, u8 val)
{
    if (addr < 0x8000) {
        cart_write(addr, val);
        return;
    } else if (addr < 0xA000) {
        printf("Err... UNSUPPORTED bus_write(%04X)\n", addr);
        NO_IMPL
    } else if (addr < 0xC000) {
        cart_write(addr, val);
    } else if (addr < 0xE000) {
        wram_write(addr, val);
    } else if (addr < 0xFE00) {
        // reserved echo RAM
    } else if (addr < 0xFEA0) {
        printf("Err... UNSUPPORTED bus_write(%04X)\n", addr);
        NO_IMPL
    } else if (addr < 0xFF00) {
        // unusable reserved
    } else if (addr < 0xFF80) {
        printf("Err... UNSUPPORTED bus_write(%04X)\n", addr);
    } else if (addr == 0xFFFF) {
        cpu_set_ie_register(val);
    } else {
        hram_write(addr, val);
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
