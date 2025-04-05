#pragma once
#include <stdint.h>

// Add flag constants
#define FLAG_Z 0x80  // Zero
#define FLAG_N 0x40  // Subtract
#define FLAG_H 0x20  // Half Carry
#define FLAG_C 0x10  // Carry

typedef struct {
    uint16_t pc;      // Program Counter
    uint16_t sp;      // Stack Pointer (16-bit)
    uint8_t a, f;     // 8-bit registers
    uint8_t b, c, d, e, h, l;
} CPU;

void cpu_init(CPU* cpu);
void cpu_step(CPU* cpu);