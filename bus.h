// bus.h
#pragma once
#include <stdint.h>
#include <stddef.h> // For size_t

// Forward declare cpu_context to avoid circular dependency
typedef struct cpu_context cpu_context;

// Unified memory array
extern uint8_t memory[0x10000];  // 64KB address space

// Function declarations
void bus_link_context(cpu_context* ctx);
uint8_t bus_read(uint16_t address);
void bus_write(uint16_t address, uint8_t value);
void bus_load_rom(uint16_t address, const uint8_t* data, size_t size);
void bus_reset(void);