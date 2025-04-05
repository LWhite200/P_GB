#include "mmu.h"
#include <stdio.h>

static uint8_t memory[0x10000]; // 64KB

uint8_t mmu_read(uint16_t addr) {
    return memory[addr];
}

void mmu_write(uint16_t addr, uint8_t val) {
    memory[addr] = val;
}

void load_rom(const char* path) {
    FILE* rom = fopen(path, "rb");
    if (!rom) {
        printf("ERROR: Could not open ROM at %s\n", path);
        return;
    }
    fread(memory + 0x100, 1, 0x8000 - 0x100, rom);
    printf("ROM loaded! First opcode: 0x%02X\n", memory[0x100]);
    fclose(rom);
}