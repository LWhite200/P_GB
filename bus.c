#include "bus.h"
#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Assuming the following arrays are defined somewhere in your project, such as in "memory.c" or elsewhere

uint8_t rom[0x8000];  // 32KB ROM (0x0000 - 0x7FFF)
uint8_t ext_ram[0x2000];  // 8KB external RAM (0xA000 - 0xBFFF)
uint8_t io_registers[0x80];  // 128 bytes of I/O registers (0xFF00 - 0xFF7F)
uint8_t hram[0x7F];  // 127 bytes of HRAM (0xFF80 - 0xFFFE)
uint8_t wram[0x2000];  // 8KB Work RAM (0xC000 - 0xDFFF)

// Function to read from ROM (0x0000 - 0x7FFF)
uint8_t rom_read(uint16_t address) {
    if (address < 0x8000) {
        return rom[address];  // Return data from ROM
    }
    printf("INVALID ROM ACCESS AT %04X\n", address);
    exit(-1);  // Invalid ROM access
}

// Function to read from external RAM (0xA000 - 0xBFFF)
uint8_t ext_ram_read(uint16_t address) {
    address -= 0xA000;  // External RAM starts at 0xA000
    if (address < 0x2000) {
        return ext_ram[address];  // Return data from external RAM
    }
    printf("INVALID EXTERNAL RAM ACCESS AT %04X\n", address);
    exit(-1);  // Invalid external RAM access
}

// Function to read from I/O registers (0xFF00 - 0xFF7F)
uint8_t io_read(uint16_t address) {
    address -= 0xFF00;  // I/O registers start at 0xFF00
    if (address < 0x80) {
        return io_registers[address];  // Return data from I/O registers
    }
    printf("INVALID I/O ACCESS AT %04X\n", address);
    exit(-1);  // Invalid I/O access
}

// Function to read from HRAM (0xFF80 - 0xFFFE)
uint8_t hram_read(uint16_t address) {
    address -= 0xFF80;  // HRAM starts at 0xFF80
    if (address < 0x7F) {
        return hram[address];  // Return data from HRAM
    }
    printf("INVALID HRAM ACCESS AT %04X\n", address);
    exit(-1);  // Invalid HRAM access
}

// Function to read from WRAM (0xC000 - 0xDFFF)
uint8_t wram_read(uint16_t address) {
    address -= 0xC000;  // WRAM starts at 0xC000
    if (address < 0x2000) {
        return wram[address];  // Return data from WRAM
    }
    printf("INVALID WRAM ACCESS AT %04X\n", address);
    exit(-1);  // Invalid WRAM access
}

// Function to read from the bus, handles different memory regions
uint8_t bus_read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x7FFF) {
        // Handle reading from ROM
        return rom_read(address);  // Calls rom_read for ROM access
    }
    else if (address >= 0xA000 && address <= 0xBFFF) {
        // Handle reading from external RAM
        return ext_ram_read(address);  // Calls ext_ram_read for external RAM
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        // Handle reading from WRAM (0xC000 - 0xDFFF)
        return wram_read(address);  // Calls wram_read for WRAM
    }
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        // Handle reading from I/O registers
        return io_read(address);  // Calls io_read for I/O access
    }
    else if (address >= 0xFF80 && address <= 0xFFFE) {
        // Handle reading from HRAM (0xFF80 - 0xFFFE)
        return hram_read(address);  // Calls hram_read for HRAM
    }

    printf("INVALID MEMORY ACCESS AT %04X\n", address);
    exit(-1);  // Invalid memory access
}

