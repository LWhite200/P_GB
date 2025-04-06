// bus.c
#include "bus.h"
#include "cpu.h"  // Add this to access cpu_context
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t memory[0x10000];  // 64KB address space

static cpu_context* bus_ctx = NULL;

// Add this initialization function
void bus_link_context(cpu_context* ctx) {
    bus_ctx = ctx;
}

void bus_reset() {
    memset(memory, 0, sizeof(memory));
}

// In bus.c
uint8_t bus_read(uint16_t addr) {

    if (addr == 0xFF0F && bus_ctx) return bus_ctx->int_flags | 0xE0;
    if (addr == 0xFFFF && bus_ctx) return bus_ctx->int_enable;

    if (addr == 0xFF44) return 0x90; // Always say we're at scanline 144
    if (addr == 0xFF0F) return 0x01; // Always say VBlank happened
    // Handle memory-mapped I/O
    if (addr >= 0xFF00 && addr <= 0xFF7F) {
        // Default values for critical registers
        if (addr == 0xFF00) return 0xCF; // Joypad
        if (addr == 0xFF04) return rand() & 0xFF; // DIV
        if (addr == 0xFF40) return 0x91; // LCDC
        return 0;
    }
    return memory[addr];
}

void bus_write(uint16_t addr, uint8_t val) {
    // Handle memory-mapped I/O first
    switch (addr) {
        // Interrupt flags
    case 0xFF0F:
        if (bus_ctx) bus_ctx->int_flags = val & 0x1F;
        return;

        // Interrupt enable
    case 0xFFFF:
        if (bus_ctx) bus_ctx->int_enable = val;
        return;

        // LCD Control
    case 0xFF40: case 0xFF41: case 0xFF42: case 0xFF43:
        case 0xFF44: case 0xFF45: case 0xFF46: case 0xFF47:
        case 0xFF48: case 0xFF49: case 0xFF4A: case 0xFF4B:
        // Handle LCD controller writes
        memory[addr] = val;
        return;

        // Timer registers
    case 0xFF04: case 0xFF05: case 0xFF06: case 0xFF07:
        memory[addr] = val;
        return;

        // Joypad
    case 0xFF00:
        memory[addr] = val;
        return;
    }

    // Protect ROM areas
    if (addr < 0x8000) {
        printf("Attempted write to ROM area: %04X\n", addr);
        return;
    }

    // Protect HRAM echo area
    if (addr >= 0xFEA0 && addr <= 0xFEFF) {
        printf("Attempted write to prohibited area: %04X\n", addr);
        return;
    }

    // Handle VRAM writes
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        // Add VRAM access checks if needed
        memory[addr] = val;
        return;
    }

    // Handle OAM writes
    if (addr >= 0xFE00 && addr <= 0xFE9F) {
        // Add OAM access checks if needed
        memory[addr] = val;
        return;
    }

    // Default case - write to memory
    memory[addr] = val;
}

void bus_load_rom(uint16_t address, const uint8_t* data, size_t size) {
    if (address + size > 0x10000) {
        printf("ROM too large for memory\n");
        exit(1);
    }
    memcpy(&memory[address], data, size);
}