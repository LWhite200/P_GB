#include "mmu.h"
#include "bus.h"
#include <string.h>

uint8_t memory[MEMORY_SIZE];

void mmu_init() {
    memset(memory, 0, MEMORY_SIZE);

    // Initialize special areas
    for (int i = 0xC000; i <= 0xDFFF; i++) memory[i] = 0x00; // WRAM
    for (int i = 0xFF80; i <= 0xFFFE; i++) memory[i] = 0x00; // HRAM

    // Default IO values
    memory[0xFF05] = 0x00; // TIMA
    memory[0xFF06] = 0x00; // TMA
    memory[0xFF07] = 0x00; // TAC
    memory[0xFF40] = 0x91; // LCDC
    memory[0xFF41] = 0x85; // STAT
    memory[0xFF47] = 0xFC; // BGP
    memory[0xFF48] = 0xFF; // OBP0
    memory[0xFF49] = 0xFF; // OBP1
}

uint8_t mmu_read(uint16_t addr) {
    // Add memory banking logic here if needed
    return memory[addr];
}

void mmu_write(uint16_t addr, uint8_t val) {
    // Handle special cases first
    switch (addr) {
    case 0xFF0F: // IF
    case 0xFFFF: // IE
        memory[addr] = val;
        return;
    }

    // Protect ROM areas
    if (addr < 0x8000) {
        printf("ROM write blocked at %04X\n", addr);
        return;
    }

    memory[addr] = val;
}