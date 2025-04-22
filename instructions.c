#include "instructions.h"

static Instruction instructions[256] = {
    [0x00] = {IN_NOP, AM_IMP, RT_NONE, RT_NONE},
    [0x01] = {IN_LD, AM_R_D16, RT_BC, RT_NONE},
    [0x02] = {IN_LD, AM_MR_R, RT_BC, RT_A},
    [0x03] = {IN_INC, AM_R, RT_BC, RT_NONE},
    // Continue with other opcodes...
    // Make sure to initialize all fields for each instruction
};

static const char* inst_names[] = {
    "NOP",
    "LD",
    "INC",
    "DEC",
    "JP",
    // Match with InstructionType enum
};

const char* get_instruction_name(InstructionType type) {
    if (type >= sizeof(inst_names) / sizeof(inst_names[0])) {
        return "UNKNOWN";
    }
    return inst_names[type];
}

Instruction* instruction_by_opcode(uint8_t opcode) {
    if (opcode >= sizeof(instructions) / sizeof(instructions[0])) {
        return NULL;
    }
    return &instructions[opcode];
}