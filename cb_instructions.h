#ifndef CB_INSTRUCTIONS_H
#define CB_INSTRUCTIONS_H

#include <stdint.h>

// Forward declaration of your CPU state struct
typedef struct CPU CPU;

// Function pointer type for CB-instruction handler
typedef void (*CBInstrHandler)(CPU* cpu, uint8_t opcode);

// Optional: You can make more specific function pointer typedefs if needed
// typedef void (*CBBitInstrHandler)(CPU* cpu, uint8_t reg);
// typedef void (*CBResInstrHandler)(CPU* cpu, uint8_t reg, uint8_t bit);

// Structure for a CB-prefixed instruction
typedef struct {
    const char* name;          // Human-readable name like "RLC B"
    CBInstrHandler execute;    // Function to execute this instruction
    uint8_t cycles;            // Cycle count
} CBInstruction;

// Declare the CB instruction table (defined in .c file)
extern CBInstruction cb_instructions[256];

#endif // CB_INSTRUCTIONS_H
