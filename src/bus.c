#include "bus.h"
#include "cart.h"
#include "ram.h"
#include "cpu.h"
#include "io.h"
#include "ppu.h"
#include "dma.h"

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
    if (addr < 0x8000) {         // ROM Data
        return cart_read(addr);
    } else if (addr < 0xA000) {  // Char/Map Data
        return ppu_vram_read(addr);
    } else if (addr < 0xC000) {  // Cart RAM
        return cart_read(addr);
    } else if (addr < 0xE000) {  // WRAM (Work RAM)
        return wram_read(addr);
    } else if (addr < 0xFE00) {  // Reverse Echo RAM
        return 0;
    } else if (addr < 0xFEA0) {  // OAM
        if (dma_transfering())
            return 0xFF;
        return ppu_oam_read(addr);
    } else if (addr < 0xFF00) {
        return 0;                // Reserved Unusable
    } else if (addr < 0xFF80) {  // IO Registers
        return io_read(addr);
    } else if (addr == 0xFFFF) { // CPU ENABLE REGISTER
        return cpu_get_ie_register();
    }
    return hram_read(addr);
}

void bus_write(u16 addr, u8 val)
{
    if (addr < 0x8000) {          // ROM Data
        cart_write(addr, val);
        return;
    } else if (addr < 0xA000) {   // Char/Map Data
        ppu_vram_write(addr, val);
    } else if (addr < 0xC000) {   // Cart RAM
        cart_write(addr, val);
    } else if (addr < 0xE000) {   // WRAM (Work RAM)
        wram_write(addr, val);
    } else if (addr < 0xFE00) {   // Reserved for Echo RAM
        // reserved echo RAM
    } else if (addr < 0xFEA0) {   // OAM
        if (dma_transfering())
            return;
        ppu_oam_write(addr, val);
    } else if (addr < 0xFF00) {
        // unusable reserved
    } else if (addr < 0xFF80) {   // IO Regisetrs
        io_write(addr, val);
    } else if (addr == 0xFFFF) {
        cpu_set_ie_register(val);
    } else {
        hram_write(addr, val);
    }
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
