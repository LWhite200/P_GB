#include "cb_instructions.h"
#include "instructions.h"  // Include this to use the instruction struct from instructions.h
#include "bus.h"
#include "cpu.h"

// Correct definition of instruction, no need to redefine it
// Use the instruction struct already defined in instructions.h

// Update resolve_reg8 to use reg_type enum values
// Updated resolve_reg8 to handle RT_HL properly
// Update resolve_reg8 to properly handle RT_HL
// Updated resolve_reg8 with better HL handling
uint8_t* resolve_reg8(cpu_context* ctx, reg_type reg) {
    switch (reg) {
    case RT_A: return &ctx->regs.a;
    case RT_B: return &ctx->regs.b;
    case RT_C: return &ctx->regs.c;
    case RT_D: return &ctx->regs.d;
    case RT_E: return &ctx->regs.e;
    case RT_H: return &ctx->regs.h;
    case RT_L: return &ctx->regs.l;
    case RT_HL: return NULL; // Special case for memory access
    default:
        printf("Invalid register access: %d\n", reg);
        return NULL;
    }
}

// Unified memory access function for HL operations
static uint8_t get_hl_value(cpu_context* ctx) {
    uint16_t addr = cpu_read_reg16(ctx, RT_HL);
    return bus_read(addr);
}

static void set_hl_value(cpu_context* ctx, uint8_t value) {
    uint16_t addr = cpu_read_reg16(ctx, RT_HL);
    bus_write(addr, value);
}


// Update execute_bit to handle HL correctly
// Updated execute_bit with better HL handling
int execute_bit(cpu_context* ctx, instruction* inst) {
    uint8_t bit = (inst->type >> 3) & 0x07;
    uint8_t value;
    int cycles = 8;

    if (inst->reg_1 == RT_HL) {
        value = get_hl_value(ctx);
        cycles = 16;
    }
    else {
        uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
        if (!reg) return -1;
        value = *reg;
    }

    ctx->regs.f &= ~(FLAG_Z | FLAG_N);
    ctx->regs.f |= FLAG_H;
    if (!(value & (1 << bit))) {
        ctx->regs.f |= FLAG_Z;
    }

    return cycles;
}


// Access memory at HL (should be handled with bus_read)
uint8_t mem_at(cpu_context* ctx, uint16_t address) {
    return bus_read(address);
}

// Modify memory at HL (should be handled with bus_write)
void mem_at_write(cpu_context* ctx, uint16_t address, uint8_t value) {
    bus_write(address, value);
}

// Updated execute_res with HL support
int execute_res(cpu_context* ctx, instruction* inst) {
    uint8_t bit = (inst->type >> 3) & 0x07;
    uint8_t mask = ~(1 << bit);

    if (inst->reg_1 == RT_HL) {
        uint8_t value = get_hl_value(ctx) & mask;
        set_hl_value(ctx, value);
        return 16;
    }

    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    if (!reg) return -1;
    *reg &= mask;
    return 8;
}

// Updated execute_set with HL support
int execute_set(cpu_context* ctx, instruction* inst) {
    uint8_t bit = (inst->type >> 3) & 0x07;
    uint8_t mask = (1 << bit);

    if (inst->reg_1 == RT_HL) {
        uint8_t value = get_hl_value(ctx) | mask;
        set_hl_value(ctx, value);
        return 16;
    }

    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    if (!reg) return -1;
    *reg |= mask;
    return 8;
}

// Updated rotation operations with HL support
int execute_rl(cpu_context* ctx, instruction* inst) {
    uint8_t old_c = (ctx->regs.f & FLAG_C) ? 1 : 0;
    uint8_t value;
    int cycles = 8;

    if (inst->reg_1 == RT_HL) {
        value = get_hl_value(ctx);
        cycles = 16;
    }
    else {
        uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
        if (!reg) return -1;
        value = *reg;
    }

    ctx->regs.f = 0;
    if (value & 0x80) ctx->regs.f |= FLAG_C;

    value = (value << 1) | old_c;
    if (value == 0) ctx->regs.f |= FLAG_Z;

    if (inst->reg_1 == RT_HL) {
        set_hl_value(ctx, value);
    }
    else {
        uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
        *reg = value;
    }

    return cycles;
}

// Rotate Right through Carry
int execute_rr(cpu_context* ctx, instruction* inst) {
    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    uint8_t old = *reg;
    *reg = (*reg >> 1) | (ctx->regs.f & FLAG_C ? 0x80 : 0);  // Carry into bit 7
    ctx->regs.f = 0;
    if (old & 0x01) ctx->regs.f |= FLAG_C;  // Set carry flag if bit 0 was 1
    if (*reg == 0) ctx->regs.f |= FLAG_Z;  // Set zero flag if result is 0

    return (inst->reg_1 == RT_HL) ? 16 : 8;  // 16 cycles for HL, 8 for others
}

// Updated swap operation with HL support
int execute_swap(cpu_context* ctx, instruction* inst) {
    uint8_t value;
    int cycles = 8;

    if (inst->reg_1 == RT_HL) {
        value = get_hl_value(ctx);
        cycles = 16;
    }
    else {
        uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
        if (!reg) return -1;
        value = *reg;
    }

    value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
    ctx->regs.f = value ? 0 : FLAG_Z;

    if (inst->reg_1 == RT_HL) {
        set_hl_value(ctx, value);
    }
    else {
        uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
        *reg = value;
    }

    return cycles;
}

// In cb_instructions.c
int execute_srl(cpu_context* ctx, instruction* inst) {
    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    uint8_t old = *reg;
    ctx->regs.f = 0;

    // Shift right by 1 bit, with no sign extension (bit 7 becomes 0)
    *reg >>= 1;
    if (old & 0x01) ctx->regs.f |= FLAG_C;  // Set carry flag if bit 0 was 1
    if (*reg == 0) ctx->regs.f |= FLAG_Z;  // Set zero flag if result is 0

    return (inst->reg_1 == RT_HL) ? 16 : 8;  // 16 cycles for HL, 8 for others
}

// In cb_instructions.c
int execute_sra(cpu_context* ctx, instruction* inst) {
    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    uint8_t old = *reg;
    ctx->regs.f = 0;

    // Shift right by 1 bit, with sign extension (bit 7 stays the same)
    *reg >>= 1;
    if (old & 0x01) ctx->regs.f |= FLAG_C;  // Set carry flag if bit 0 was 1
    if (*reg == 0) ctx->regs.f |= FLAG_Z;  // Set zero flag if result is 0

    return (inst->reg_1 == RT_HL) ? 16 : 8;  // 16 cycles for HL, 8 for others
}



// Example of shift operation implementation
int execute_sla(cpu_context* ctx, instruction* inst) {
    uint8_t* reg = resolve_reg8(ctx, inst->reg_1);
    uint8_t old = *reg;
    *reg <<= 1;
    ctx->regs.f = 0;
    if (old & 0x80) ctx->regs.f |= FLAG_C; // Carry flag set if bit 7 was 1
    if (*reg == 0) ctx->regs.f |= FLAG_Z; // Zero flag set if result is 0
    return (inst->reg_1 == RT_HL) ? 16 : 8; // 16 cycles for HL address, 8 for others
}

// Similarly for other CB instructions
int execute_cb_instruction(cpu_context* ctx, uint8_t cb_opcode) {
    printf("[CB %02X] ", cb_opcode); // Debug output

    // Operation groups
    static const char* op_names[4] = { "ROT/SHIFT", "BIT", "RES", "SET" };
    uint8_t op = cb_opcode >> 6;          // Top 2 bits: operation group
    uint8_t bit = (cb_opcode >> 3) & 0x07; // Bit position (or operation ID)
    uint8_t reg_id = cb_opcode & 0x07;     // Register or HL

    // Map register ID to reg_type
    static const reg_type cb_reg_map[8] = {
        RT_B, RT_C, RT_D, RT_E, RT_H, RT_L, RT_HL, RT_A
    };

    if (reg_id > 7) {
        printf("Invalid CB register ID: %d\n", reg_id);
        return -1;
    }

    reg_type rt = cb_reg_map[reg_id];
    printf("%s %d,%s - ", op_names[op], bit, reg_type_to_str(rt));

    int cycles = (rt == RT_HL) ? 16 : 8;
    uint8_t value;
    uint16_t addr = 0;

    // Get the value
    if (rt == RT_HL) {
        addr = cpu_read_reg16(ctx, RT_HL);
        value = bus_read(addr);
        printf("[HL]=%02X @ %04X - ", value, addr);
    }
    else {
        value = cpu_read_reg8(ctx, rt);
        printf("%s=%02X - ", reg_type_to_str(rt), value);
    }

    // Handle operations
    switch (op) {
    case 0: { // Rotate/Shift group
        uint8_t old_value = value;
        const char* op_name = "";

        switch (bit) {
        case 0: // RLC
            op_name = "RLC";
            ctx->regs.f = (value & 0x80) ? FLAG_C : 0;
            value = (value << 1) | (value >> 7);
            break;
        case 1: // RRC
            op_name = "RRC";
            ctx->regs.f = (value & 0x01) ? FLAG_C : 0;
            value = (value >> 1) | ((value & 1) << 7);
            break;
        case 2: { // RL
            op_name = "RL";
            uint8_t carry = (ctx->regs.f & FLAG_C) ? 1 : 0;
            ctx->regs.f = (value & 0x80) ? FLAG_C : 0;
            value = (value << 1) | carry;
            break;
        }
        case 3: { // RR
            op_name = "RR";
            uint8_t carry = (ctx->regs.f & FLAG_C) ? 0x80 : 0;
            ctx->regs.f = (value & 0x01) ? FLAG_C : 0;
            value = (value >> 1) | carry;
            break;
        }
        case 4: // SLA
            op_name = "SLA";
            ctx->regs.f = (value & 0x80) ? FLAG_C : 0;
            value <<= 1;
            break;
        case 5: // SRA
            op_name = "SRA";
            ctx->regs.f = (value & 0x01) ? FLAG_C : 0;
            value = (value >> 1) | (value & 0x80);
            break;
        case 6: // SWAP
            op_name = "SWAP";
            ctx->regs.f = 0;
            value = (value >> 4) | (value << 4);
            break;
        case 7: // SRL
            op_name = "SRL";
            ctx->regs.f = (value & 0x01) ? FLAG_C : 0;
            value >>= 1;
            break;
        }

        printf("%s: %02X->%02X ", op_name, old_value, value);
        if (value == 0) ctx->regs.f |= FLAG_Z;
        break;
    }

    case 1: // BIT
        printf("BIT %d: ", bit);
        ctx->regs.f &= ~(FLAG_Z | FLAG_N);
        ctx->regs.f |= FLAG_H;
        if (!(value & (1 << bit))) {
            ctx->regs.f |= FLAG_Z;
            printf("0 ");
        }
        else {
            printf("1 ");
        }
        printf("F=%02X", ctx->regs.f);
        return cycles; // No write-back for BIT

    case 2: // RES
        printf("RES %d: %02X->%02X ", bit, value, value & ~(1 << bit));
        value &= ~(1 << bit);
        break;

    case 3: // SET
        printf("SET %d: %02X->%02X ", bit, value, value | (1 << bit));
        value |= (1 << bit);
        break;
    }

    // Write result back (except for BIT operations)
    if (op != 1) {
        if (rt == RT_HL) {
            bus_write(addr, value);
            printf("to [HL]");
        }
        else {
            cpu_write_reg8(ctx, rt, value);
            printf("to %s", reg_type_to_str(rt));
        }
    }

    printf(" (cycles:%d)\n", cycles);
    return cycles;
}