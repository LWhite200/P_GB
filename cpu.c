#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

// Removed global ctx, it's handled by main() now

int total_cycles = 0;

// Array for register names (optional for debugging)
const char* register_names[] = {
    "A", "B", "C", "D", "E", "H", "L", "F", "BC", "DE", "HL", "SP", "PC"
};

uint8_t cpu_read_reg(cpu_context* ctx, int reg) {
    if (reg < 0 || reg > RT_PC) {  // Check if the register index is valid
        printf("Error: Invalid register access %d\n", reg);
        exit(1);
    }

    // Debugging output to print accessed register
    printf("Accessing Register: %s (Index: %d)\n", register_names[reg], reg);

    switch (reg) {
    case RT_A: return ctx->regs.a;
    case RT_B: return ctx->regs.b;
    case RT_C: return ctx->regs.c;
    case RT_D: return ctx->regs.d;
    case RT_E: return ctx->regs.e;
    case RT_H: return ctx->regs.h;
    case RT_L: return ctx->regs.l;
    case RT_F: return ctx->regs.f;
    case RT_BC: return ctx->regs.bc;
    case RT_DE: return ctx->regs.de;
    case RT_HL: return ctx->regs.hl;
    case RT_SP: return ctx->regs.sp;
    case RT_PC: return ctx->regs.pc;
    default:
        printf("Error: Invalid register access %d\n", reg);
        exit(1);
    }
}

void cpu_set_reg(cpu_context* ctx, int reg, uint8_t value) {
    if (reg < 0 || reg > RT_PC) {  // Check if the register index is valid
        printf("Error: Invalid register access %d\n", reg);
        exit(1);
    }

    // Debugging output to print set register
    printf("Setting Register: %s (Index: %d) to Value: %d\n", register_names[reg], reg, value);

    switch (reg) {
    case RT_A: ctx->regs.a = value; break;
    case RT_B: ctx->regs.b = value; break;
    case RT_C: ctx->regs.c = value; break;
    case RT_D: ctx->regs.d = value; break;
    case RT_E: ctx->regs.e = value; break;
    case RT_H: ctx->regs.h = value; break;
    case RT_L: ctx->regs.l = value; break;
    case RT_F: ctx->regs.f = value; break;
    case RT_BC: ctx->regs.bc = value; break;
    case RT_DE: ctx->regs.de = value; break;
    case RT_HL: ctx->regs.hl = value; break;
    case RT_SP: ctx->regs.sp = value; break;
    case RT_PC: ctx->regs.pc = value; break;
    default:
        printf("Error: Invalid register access %d\n", reg);
        exit(1);
    }
}



// Emulate cycles, used for timing purposes
void emu_cycles(int cycles) {
    total_cycles += cycles;
    // Optionally print or return the cycle count if needed
}
