#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

typedef enum {
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_JP,
    // Add other instruction types as needed
} InstructionType;

typedef enum {
    AM_IMP,    // Implied
    AM_R_D16,  // Register, 16-bit data
    AM_MR_R,   // Memory at register, register
    AM_R,
    // Add other addressing modes as needed
} AddressingMode;

typedef enum {
    RT_NONE,   // No register
    RT_A,      // Register A
    RT_B,      // Register B
    RT_C,      // Register C
    RT_D,      // Register D
    RT_E,      // Register E
    RT_H,      // Register H
    RT_L,      // Register L
    RT_AF,     // Register pair AF
    RT_BC,     // Register pair BC
    RT_DE,     // Register pair DE
    RT_HL,     // Register pair HL
    RT_SP,     // Stack pointer
    RT_PC      // Program counter
} RegisterType;

typedef struct {
    InstructionType type;
    AddressingMode mode;
    RegisterType reg_1;
    RegisterType reg_2;
} Instruction;

const char* get_instruction_name(InstructionType type);
Instruction* instruction_by_opcode(uint8_t opcode);

#endif