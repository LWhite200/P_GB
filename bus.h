#pragma once
#include <stdint.h>

// Memory regions for different addresses
extern uint8_t rom[0x8000];           // 32KB ROM (0x0000 - 0x7FFF)
extern uint8_t ext_ram[0x2000];       // 8KB External RAM (0xA000 - 0xBFFF)
extern uint8_t io_registers[0x80];    // 128 bytes I/O registers (0xFF00 - 0xFF7F)
extern uint8_t hram[0x7F];            // 127 bytes HRAM (0xFF80 - 0xFFFE)
extern uint8_t wram[0x2000];          // 8KB Work RAM (0xC000 - 0xDFFF)

// Function declarations for reading memory
uint8_t bus_read(uint16_t address);   // General bus read function

// Helper functions for reading specific memory regions
uint8_t rom_read(uint16_t address);   // Read from ROM
uint8_t ext_ram_read(uint16_t address);  // Read from External RAM
uint8_t io_read(uint16_t address);    // Read from I/O registers
uint8_t hram_read(uint16_t address);  // Read from HRAM
uint8_t wram_read(uint16_t address);  // Read from WRAM
