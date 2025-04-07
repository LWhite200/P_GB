#include "cpu.h"
#include "bus.h"
#include <stdio.h>
#include <string.h>
#include "cb_instructions.h"

// memory.c or debug.c
void dump_memory(uint16_t start, uint16_t length) {
    printf("\nMemory dump from %04X to %04X:\n", start, start + length - 1);
    for (uint16_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("%04X: ", start + i);
        }
        printf("%02X ", bus_read(start + i));
        if (i % 16 == 15 || i == length - 1) {
            printf("\n");
        }
    }
}

// Initialize CPU registers
void cpu_init(cpu_context* ctx) {
    memset(ctx, 0, sizeof(cpu_context));

    // Set registers to actual Game Boy boot values
    ctx->regs.pc = 0x0100;  // Execution starts here
    ctx->regs.sp = 0xFFFE;  // Stack pointer initial value

    // Actual Game Boy boot register states
    ctx->regs.a = 0x01;     // Boot ROM sets these
    ctx->regs.f = 0xB0;     // Z=1, N=0, H=1, C=1
    ctx->regs.b = 0x00;
    ctx->regs.c = 0x13;
    ctx->regs.d = 0x00;
    ctx->regs.e = 0xD8;
    ctx->regs.h = 0x01;
    ctx->regs.l = 0x4D;

    // Interrupt handling
    ctx->int_enable = 0x00;  // All interrupts disabled initially
    ctx->int_flags = 0xE1;   // Typical power-on state
    ctx->interrupts_master_enable = false;
    ctx->halted = false;
    ctx->stopped = false;

    // Timing
    ctx->clock.cycles = 0;
}

// Helper function to read 16-bit value from memory
static uint16_t read16(cpu_context* ctx, uint16_t addr) {
    uint16_t lo = bus_read(addr);
    uint16_t hi = bus_read(addr + 1);
    return (hi << 8) | lo;
}

// Helper function to write 16-bit value to memory
static void write16(cpu_context* ctx, uint16_t addr, uint16_t val) {
    bus_write(addr, val & 0xFF);
    bus_write(addr + 1, val >> 8);
}

// Fetch the next instruction
uint8_t cpu_fetch(cpu_context* ctx) {
    uint8_t opcode = bus_read(ctx->regs.pc++);
    ctx->opcode = opcode;
    return opcode;
}

// Add these implementations before cpu_step() function

static int execute_ret(cpu_context* ctx, instruction* inst) {
    // Pop return address from stack
    uint16_t lo = bus_read(ctx->regs.sp++);
    uint16_t hi = bus_read(ctx->regs.sp++);
    ctx->regs.pc = (hi << 8) | lo;

    // Conditional RET checks flag condition
    if (inst->cond != CT_NONE) {
        bool z = (ctx->regs.f & FLAG_Z) != 0;
        bool c = (ctx->regs.f & FLAG_C) != 0;
        bool should_ret = false;

        switch (inst->cond) {
        case CT_NZ: should_ret = !z; break;
        case CT_Z:  should_ret = z; break;
        case CT_NC: should_ret = !c; break;
        case CT_C:  should_ret = c; break;
        default: break;
        }

        if (!should_ret) {
            // Didn't take the return, undo stack pop
            ctx->regs.sp -= 2;
            return 8;
        }
    }

    return 20; // 20 cycles for RET, 8 for conditional RET not taken
}

static int execute_adc(cpu_context* ctx, instruction* inst) {
    uint8_t a = cpu_read_reg8(ctx, RT_A);
    uint8_t b = cpu_read_reg8(ctx, inst->reg_2);
    uint8_t carry = (ctx->regs.f & FLAG_C) ? 1 : 0;
    uint16_t res = a + b + carry;

    // Set flags
    ctx->regs.f = 0;
    if ((res & 0xFF) == 0) ctx->regs.f |= FLAG_Z;
    if (res > 0xFF) ctx->regs.f |= FLAG_C;
    if ((a & 0xF) + (b & 0xF) + carry > 0xF) ctx->regs.f |= FLAG_H;

    cpu_write_reg8(ctx, RT_A, (uint8_t)res);
    return 4;
}

static int execute_sub(cpu_context* ctx, instruction* inst) {
    uint8_t a = cpu_read_reg8(ctx, RT_A);
    uint8_t b = cpu_read_reg8(ctx, inst->reg_2);
    int res = a - b;

    // Set flags
    ctx->regs.f = FLAG_N;
    if ((res & 0xFF) == 0) ctx->regs.f |= FLAG_Z;
    if (res < 0) ctx->regs.f |= FLAG_C;
    if ((a & 0xF) < (b & 0xF)) ctx->regs.f |= FLAG_H;

    cpu_write_reg8(ctx, RT_A, (uint8_t)res);
    return 4;
}

static int execute_add(cpu_context* ctx, instruction* inst) {
    uint8_t a = cpu_read_reg8(ctx, RT_A);
    uint8_t b = cpu_read_reg8(ctx, inst->reg_2);
    uint16_t res = a + b;

    // Set flags
    ctx->regs.f = 0;
    if ((res & 0xFF) == 0) ctx->regs.f |= FLAG_Z;
    if (res > 0xFF) ctx->regs.f |= FLAG_C;
    if ((a & 0xF) + (b & 0xF) > 0xF) ctx->regs.f |= FLAG_H;

    cpu_write_reg8(ctx, RT_A, (uint8_t)res);
    return 4;
}

static int execute_and(cpu_context* ctx, instruction* inst) {
    uint8_t val = cpu_read_reg8(ctx, inst->reg_2);
    uint8_t res = ctx->regs.a & val;

    // Set flags
    ctx->regs.f = FLAG_H;  // Half carry flag is set
    if (res == 0) {
        ctx->regs.f |= FLAG_Z;  // Zero flag set if result is zero
    }

    cpu_write_reg8(ctx, RT_A, res);
    return 4;
}

static int execute_or(cpu_context* ctx, instruction* inst) {
    uint8_t val = cpu_read_reg8(ctx, inst->reg_2);
    uint8_t res = ctx->regs.a | val;

    // Set flags
    ctx->regs.f = 0;
    if (res == 0) ctx->regs.f |= FLAG_Z;

    cpu_write_reg8(ctx, RT_A, res);
    return 4;
}

static int execute_xor(cpu_context* ctx, instruction* inst) {
    uint8_t val = cpu_read_reg8(ctx, inst->reg_2);
    uint8_t res = ctx->regs.a ^ val;

    // Set flags
    ctx->regs.f = 0;
    if (res == 0) ctx->regs.f |= FLAG_Z;

    cpu_write_reg8(ctx, RT_A, res);
    return 4;
}

static int execute_cp(cpu_context* ctx, instruction* inst) {
    uint8_t a = cpu_read_reg8(ctx, RT_A);
    uint8_t b = cpu_read_reg8(ctx, inst->reg_2);
    int res = a - b;

    // Set flags
    ctx->regs.f = FLAG_N;
    if ((res & 0xFF) == 0) ctx->regs.f |= FLAG_Z;
    if (res < 0) ctx->regs.f |= FLAG_C;
    if ((a & 0xF) < (b & 0xF)) ctx->regs.f |= FLAG_H;

    return 4;
}

// Example for INC instruction
static int execute_inc(cpu_context* ctx, instruction* inst) {
    uint8_t val = 0;
    uint8_t res = 0;
    int cycles = 4;

    if (inst->mode == AM_R) {
        // Handle register increment (INC r8)
        val = cpu_read_reg8(ctx, inst->reg_1);
        res = val + 1;
        cpu_write_reg8(ctx, inst->reg_1, res);
    }
    else if (inst->mode == AM_MR) {
        // Handle memory increment (INC (HL))
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        val = bus_read(addr);
        res = val + 1;
        bus_write(addr, res);
        cycles = 12;
    }
    else {
        printf("Invalid INC mode: %d\n", inst->mode);
        return -1;
    }

    // Set flags
    ctx->regs.f &= FLAG_C; // Keep carry flag
    ctx->regs.f &= ~(FLAG_N | FLAG_Z | FLAG_H); // Clear other flags
    if (res == 0) ctx->regs.f |= FLAG_Z;
    if ((val & 0x0F) == 0x0F) ctx->regs.f |= FLAG_H;

    return cycles;
}

static int execute_dec(cpu_context* ctx, instruction* inst) {
    uint8_t val;
    uint8_t res;

    if (inst->mode == AM_R) {
        // DEC r
        val = cpu_read_reg8(ctx, inst->reg_1);
        res = val - 1;
        cpu_write_reg8(ctx, inst->reg_1, res);

        // Set flags
        ctx->regs.f &= FLAG_C; // Keep carry flag
        if (res == 0) ctx->regs.f |= FLAG_Z;
        ctx->regs.f |= FLAG_N;
        if ((val & 0xF) == 0x0) ctx->regs.f |= FLAG_H;

        return 4;
    }
    else if (inst->mode == AM_MR) {
        // DEC (HL)
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        val = bus_read(addr);
        res = val - 1;
        bus_write(addr, res);

        // Set flags
        ctx->regs.f &= FLAG_C; // Keep carry flag
        if (res == 0) ctx->regs.f |= FLAG_Z;
        ctx->regs.f |= FLAG_N;
        if ((val & 0xF) == 0x0) ctx->regs.f |= FLAG_H;

        return 12;
    }

    printf("Invalid DEC mode\n");
    return -1;
}

/// idk what this stuff below is but just go with it
static int execute_rlc(cpu_context* ctx, instruction* inst) {
    uint8_t val = cpu_read_reg8(ctx, inst->reg_1);
    uint8_t bit7 = (val >> 7) & 1;
    val = (val << 1) | bit7;

    ctx->regs.f = 0;
    if (bit7) ctx->regs.f |= FLAG_C;
    if (val == 0) ctx->regs.f |= FLAG_Z;

    cpu_write_reg8(ctx, inst->reg_1, val);
    return inst->reg_1 == RT_A ? 4 : 8;
}

static int execute_rl(cpu_context* ctx, instruction* inst) {
    uint8_t value;
    uint8_t carry_in = (ctx->regs.f & FLAG_C) ? 1 : 0;
    int cycles = 8;

    if (inst->reg_2 == RT_HL && inst->mode == AM_MR) {
        // RL (HL)
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        value = bus_read(addr);
        cycles = 16;
    }
    else {
        // RL r8
        value = cpu_read_reg8(ctx, inst->reg_2);
    }

    uint8_t carry_out = (value >> 7) & 1;
    value = (value << 1) | carry_in;

    // Set flags
    ctx->regs.f = 0;
    if (value == 0) ctx->regs.f |= FLAG_Z;
    if (carry_out) ctx->regs.f |= FLAG_C;

    // Store result
    if (inst->reg_2 == RT_HL && inst->mode == AM_MR) {
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        bus_write(addr, value);
    }
    else {
        cpu_write_reg8(ctx, inst->reg_2, value);
    }

    return cycles;
}

// Special case for RLA (RL A but don't set Z flag)
static int execute_rla(cpu_context* ctx) {
    uint8_t carry_in = (ctx->regs.f & FLAG_C) ? 1 : 0;
    uint8_t carry_out = (ctx->regs.a >> 7) & 1;

    // Rotate left through carry
    ctx->regs.a = (ctx->regs.a << 1) | carry_in;

    // Set flags - RLA only affects Carry flag
    ctx->regs.f &= ~(FLAG_C | FLAG_N | FLAG_H);  // Clear these flags
    if (carry_out) {
        ctx->regs.f |= FLAG_C;  // Set carry if bit 7 was set
    }

    return 4;  // 4 cycles for RLA
}

static int execute_rrc(cpu_context* ctx, instruction* inst) {
    uint8_t val = cpu_read_reg8(ctx, inst->reg_1);
    uint8_t bit0 = val & 1;
    val = (val >> 1) | (bit0 << 7);

    ctx->regs.f = 0;
    if (bit0) ctx->regs.f |= FLAG_C;
    if (val == 0) ctx->regs.f |= FLAG_Z;

    cpu_write_reg8(ctx, inst->reg_1, val);
    return inst->reg_1 == RT_A ? 4 : 8;
}


static int execute_bit(cpu_context* ctx, instruction* inst) {
    uint8_t bit = inst->reg_1; // reg_1 contains the bit number (0-7)
    uint8_t value;
    int cycles = 8;

    if (inst->reg_2 == RT_HL && inst->mode == AM_MR) {
        // BIT n,(HL)
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        value = bus_read(addr);
        cycles = 16;
    }
    else {
        // BIT n,r
        value = cpu_read_reg8(ctx, inst->reg_2);
    }

    // Set flags
    ctx->regs.f &= ~(FLAG_Z | FLAG_N);
    ctx->regs.f |= FLAG_H;  // H flag always set
    if (!(value & (1 << bit))) {
        ctx->regs.f |= FLAG_Z;  // Z set if bit is 0
    }

    return cycles;
}


static int execute_cpl(cpu_context* ctx) {
    ctx->regs.a = ~ctx->regs.a;
    ctx->regs.f |= FLAG_N | FLAG_H; // Set N and H flags
    return 4;
}

static int execute_ccf(cpu_context* ctx) {
    ctx->regs.f ^= FLAG_C; // Toggle carry
    ctx->regs.f &= ~(FLAG_N | FLAG_H); // Clear N and H
    return 4;
}

static int execute_scf(cpu_context* ctx) {
    ctx->regs.f |= FLAG_C; // Set carry
    ctx->regs.f &= ~(FLAG_N | FLAG_H); // Clear N and H
    return 4;
}

static int execute_rst(cpu_context* ctx, instruction* inst) {
    ctx->regs.sp -= 2;
    write16(ctx, ctx->regs.sp, ctx->regs.pc);
    ctx->regs.pc = inst->param; // RST addresses are fixed (0x00,0x08,...,0x38)
    return 16;
}

static int execute_daa(cpu_context* ctx) {
    uint16_t a = ctx->regs.a;

    if (ctx->regs.f & FLAG_N) { // After subtraction
        if (ctx->regs.f & FLAG_H) a = (a - 0x06) & 0xFF;
        if (ctx->regs.f & FLAG_C) a -= 0x60;
    }
    else { // After addition
        if ((a & 0x0F) > 0x09 || (ctx->regs.f & FLAG_H)) a += 0x06;
        if (a > 0x9F || (ctx->regs.f & FLAG_C)) a += 0x60;
    }

    ctx->regs.f &= ~FLAG_H;
    if (a & 0x100) ctx->regs.f |= FLAG_C;
    ctx->regs.a = a & 0xFF;
    ctx->regs.f = (ctx->regs.f & ~FLAG_Z) | (ctx->regs.a ? 0 : FLAG_Z);
    return 4;
}

static int execute_stop(cpu_context* ctx) {
    // Handle STOP (enter low-power mode)
    ctx->stopped = true;
    return 4;
}

static int execute_halt(cpu_context* ctx) {
    // Handle HALT (wait for interrupt)
    ctx->halted = true;
    return 4;
}

static int execute_ei(cpu_context* ctx) {
    ctx->ime = true; // Enable interrupts after next instruction
    return 4;
}

static int execute_di(cpu_context* ctx) {
    ctx->ime = false; // Disable interrupts immediately
    return 4;
}


// Execute a single instruction
// Execute a single instruction

static void cpu_push(cpu_context* ctx, uint16_t val) {
    ctx->regs.sp -= 2;          // Move stack pointer down
    bus_write(ctx->regs.sp + 1, val >> 8);    // Write high byte
    bus_write(ctx->regs.sp, val & 0xFF);      // Write low byte
}

int execute_jr(cpu_context* ctx, instruction* inst) {
    int8_t offset = cpu_fetch(ctx); // Fetch the signed 8-bit offset
    ctx->regs.pc += offset; // Apply the offset to the PC
    return 12; // JR takes 12 cycles
}


int execute_ldh(cpu_context* ctx, uint8_t opcode) {
    uint8_t address;

    if (opcode == 0xE0) { // LDH (0xFF00 + d8), A
        address = cpu_fetch(ctx); // Fetch the 8-bit address (d8)
        bus_write(0xFF00 + address, ctx->regs.a); // Write A to the I/O port (0xFF00 + d8)
        ctx->regs.pc++; // Increment PC after handling the instruction
        return 12; // LDH (0xFF00 + d8), A takes 12 cycles
    }
    else if (opcode == 0xF0) { // LDH A, (0xFF00 + d8)
        address = cpu_fetch(ctx); // Fetch the 8-bit address (d8)
        ctx->regs.a = bus_read(0xFF00 + address); // Read the value from the I/O port (0xFF00 + d8) into A
        ctx->regs.pc++; // Increment PC after handling the instruction
        return 12; // LDH A, (0xFF00 + d8) takes 12 cycles
    }

    return -1; // Unknown LDH opcode
}




int execute_rrca(cpu_context* ctx) {
    uint8_t carry = ctx->regs.f & 0x10; // Get the current carry flag (bit 4 in F register)
    uint8_t new_carry = ctx->regs.a & 0x01; // Get the least significant bit of A to set as the new carry

    ctx->regs.a = (ctx->regs.a >> 1) | (carry << 7); // Shift A right by 1 and put the old carry into bit 7
    ctx->regs.f = (ctx->regs.f & 0xF0) | (new_carry ? 0x10 : 0x00); // Update the carry flag

    return 4; // RRCA takes 4 cycles
}


static int execute_ld(cpu_context* ctx, instruction* inst) {
    // Register validation
#define VALID_REG8(r) ((r) >= RT_A && (r) <= RT_HL)
#define VALID_REG16(r) ((r) >= RT_AF && (r) <= RT_PC)

    if ((inst->mode == AM_R_R || inst->mode == AM_R_MR || inst->mode == AM_MR_R) &&
        (!VALID_REG8(inst->reg_1) || !VALID_REG8(inst->reg_2))) {
        printf("INVALID LD: mode=%d reg1=%d reg2=%d at PC=%04X\n",
            inst->mode, inst->reg_1, inst->reg_2, ctx->regs.pc);
        return -1;
    }

    uint16_t addr;
    uint8_t val8;
    uint16_t val16;
    int cycles = 4;

    printf("LD %s,%s (mode=%d) @ PC=%04X: ",
        reg_type_to_str(inst->reg_1),
        inst->mode == AM_R_D8 ? "d8" :
        inst->mode == AM_MR_D8 ? "d8" :
        inst->mode == AM_A8_R ? "(a8)" :
        inst->mode == AM_R_A8 ? "A,(a8)" :
        reg_type_to_str(inst->reg_2),
        inst->mode,
        ctx->regs.pc);

    switch (inst->mode) {
    case AM_R_R: // LD r,r'
        val8 = cpu_read_reg8(ctx, inst->reg_2);
        cpu_write_reg8(ctx, inst->reg_1, val8);
        printf("%s=%02X->%s",
            reg_type_to_str(inst->reg_2), val8,
            reg_type_to_str(inst->reg_1));
        break;

    case AM_R_D8: // LD r,d8
        val8 = bus_read(ctx->regs.pc++);
        cpu_write_reg8(ctx, inst->reg_1, val8);
        printf("d8=%02X->%s", val8, reg_type_to_str(inst->reg_1));
        cycles = 8;
        break;

    case AM_R_MR: { // LD r,(HL)
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = bus_read(addr);
        cpu_write_reg8(ctx, inst->reg_1, val8);
        printf("[%04X]=%02X->%s", addr, val8, reg_type_to_str(inst->reg_1));
        cycles = 8;
        break;
    }

    case AM_MR_R: { // LD (HL),r
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = cpu_read_reg8(ctx, inst->reg_2);
        bus_write(addr, val8);
        printf("%s=%02X->[%04X]",
            reg_type_to_str(inst->reg_2), val8, addr);
        cycles = 8;
        break;
    }

    case AM_MR_D8: // LD (HL),d8
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = bus_read(ctx->regs.pc++);
        bus_write(addr, val8);
        cycles = 12;
        break;

    case AM_A8_R: // LDH (a8),A
        addr = 0xFF00 | bus_read(ctx->regs.pc++);
        val8 = cpu_read_reg8(ctx, RT_A);
        bus_write(addr, val8);
        cycles = 12;
        break;

    case AM_R_A8: // LDH A,(a8)
        addr = 0xFF00 | bus_read(ctx->regs.pc++);
        val8 = bus_read(addr);
        cpu_write_reg8(ctx, RT_A, val8);
        cycles = 12;
        break;

    case AM_HLI_R: // LD (HL+),A
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = cpu_read_reg8(ctx, RT_A);
        bus_write(addr, val8);
        cpu_write_reg16(ctx, RT_HL, addr + 1);
        cycles = 8;
        break;

    case AM_HLD_R: // LD (HL-),A
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = cpu_read_reg8(ctx, RT_A);
        bus_write(addr, val8);
        cpu_write_reg16(ctx, RT_HL, addr - 1);
        cycles = 8;
        break;

    case AM_R_HLI: // LD A,(HL+)
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = bus_read(addr);
        cpu_write_reg8(ctx, RT_A, val8);
        cpu_write_reg16(ctx, RT_HL, addr + 1);
        cycles = 8;
        break;

    case AM_R_HLD: // LD A,(HL-)
        addr = cpu_read_reg16(ctx, RT_HL);
        val8 = bus_read(addr);
        cpu_write_reg8(ctx, RT_A, val8);
        cpu_write_reg16(ctx, RT_HL, addr - 1);
        cycles = 8;
        break;

        // 16-bit Loads -------------------------------------------------------
    case AM_R_D16: // LD rr,d16
        val16 = read16(ctx, ctx->regs.pc);
        ctx->regs.pc += 2;
        cpu_write_reg16(ctx, inst->reg_1, val16);
        cycles = 12;
        break;

    case AM_R_A16: // LD A,(a16)
        addr = read16(ctx, ctx->regs.pc);
        ctx->regs.pc += 2;
        val8 = bus_read(addr);
        cpu_write_reg8(ctx, RT_A, val8);
        cycles = 16;
        break;

    case AM_A16_R: // LD (a16),A
        addr = read16(ctx, ctx->regs.pc);
        ctx->regs.pc += 2;
        val8 = cpu_read_reg8(ctx, RT_A);
        bus_write(addr, val8);
        cycles = 16;
        break;

    case AM_HL_SPR: // LD HL,SP+r8
    {
        uint16_t sp = cpu_read_reg16(ctx, RT_SP);
        int8_t offset = (int8_t)bus_read(ctx->regs.pc++);
        uint16_t res = sp + offset;

        // Set flags
        ctx->regs.f = 0;
        if ((sp ^ offset ^ res) & 0x10) ctx->regs.f |= FLAG_H;
        if ((sp ^ offset ^ res) & 0x100) ctx->regs.f |= FLAG_C;

        cpu_write_reg16(ctx, RT_HL, res);
        cycles = 12;
        break;
    }

    // ... (other cases remain similar but with added debug prints) ...

    default:
        printf("UNIMPLEMENTED LD MODE: %d\n", inst->mode);
        return -1;
    }

    printf(" (cycles:%d)\n", cycles);
    return cycles;
}




// Execute a single instruction
// Execute a single instruction
// Execute a single instruction
int cpu_step(cpu_context* ctx) {
    uint8_t opcode = cpu_fetch(ctx); // Fetch the opcode from memory
    printf("PC:%04X OP:%02X\n", ctx->regs.pc, bus_read(ctx->regs.pc));

    // Handle interrupts and other special operations (same as your code above)

    // Handle CB-prefixed instructions
    if (opcode == 0xCB) {
        uint8_t cb_opcode = cpu_fetch(ctx); // Fetch the CB-prefixed opcode
        ctx->regs.pc++;  // Increment the program counter for CB instructions

        // Execute the CB-prefixed instruction
        int cycles = execute_cb_instruction(ctx, cb_opcode);
        if (cycles == -1) {
            printf("Unknown CB opcode: CB %02X at PC=%04X\n", cb_opcode, ctx->regs.pc - 1);
            return -1;
        }

        return cycles;  // Return the number of cycles for the CB instruction
    }

    // Handle regular instructions (non-CB-prefixed)
    instruction* inst = instruction_by_opcode(opcode); // Get instruction by opcode

    if (!inst) {
        printf("Unknown opcode: %02X at PC=%04X\n", opcode, ctx->regs.pc - 1);
        return -1;
    }

    ctx->cur_inst = inst;

    // Execute the instruction and update PC accordingly
    int cycles = 0;
    switch (inst->type) {
    case IN_NOP:   ctx->regs.pc++; return 4;
    case IN_LD:    cycles = execute_ld(ctx, inst); break;
    case IN_ADD:   cycles = execute_add(ctx, inst); break;
    case IN_RET:   cycles = execute_ret(ctx, inst); break;
    case IN_ADC:   cycles = execute_adc(ctx, inst); break;
    case IN_SUB:   cycles = execute_sub(ctx, inst); break;
    case IN_JR:    cycles = execute_jr(ctx, inst); break; // JR instruction
    case IN_AND:   return execute_and(ctx, inst);
    case IN_OR:    return execute_or(ctx, inst);
    case IN_XOR:   return execute_xor(ctx, inst);
    case IN_CP:    return execute_cp(ctx, inst);
    case IN_INC:   return execute_inc(ctx, inst);
    case IN_DEC:   return execute_dec(ctx, inst);
    case IN_JP:    return execute_jp(ctx, inst);
    case IN_CALL:  return execute_call(ctx, inst);
    case IN_PUSH:  return execute_push(ctx, inst);
    case IN_POP:   return execute_pop(ctx, inst);
    case IN_RLC:   return execute_rlc(ctx, inst);
    case IN_RRC:   return execute_rrc(ctx, inst);
    case IN_RLA:   return execute_rla(ctx);
    case IN_BIT:   return execute_bit(ctx, inst);
    case IN_CCF:   return execute_ccf(ctx);
    case IN_SCF:   return execute_scf(ctx);
    case IN_CPL:   return execute_cpl(ctx);
    case IN_DAA:   return execute_daa(ctx);
    case IN_RST:   return execute_rst(ctx, inst);
    case IN_EI:    return execute_ei(ctx);
    case IN_DI:    return execute_di(ctx);
    case IN_STOP:  return execute_stop(ctx);
    case IN_HALT:  return execute_halt(ctx);
    case IN_LDH:   return execute_ldh(ctx, opcode); // Pass opcode directly to execute_ldh
    case IN_RRCA:  return execute_rrca(ctx); // RRCA instruction
        // Handle other instructions...

    default:
        printf("Unimplemented instruction: %s\n", inst_name(inst->type));
        return -1;
    }

    // Ensure PC is updated correctly for all instructions
    ctx->regs.pc += (inst->length - 1); // Update PC to reflect instruction length

    return cycles;
}


static int execute_push(cpu_context* ctx, instruction* inst) {
    uint16_t val = cpu_read_reg16(ctx, inst->reg_1);
    ctx->regs.sp -= 2;
    write16(ctx, ctx->regs.sp, val);
    return 16;
}






static int execute_jp(cpu_context* ctx, instruction* inst) {
    uint16_t addr;
    bool should_jump = true;

    // Handle conditional jumps
    if (inst->cond != CT_NONE) {
        bool z = (ctx->regs.f & FLAG_Z) != 0;
        bool c = (ctx->regs.f & FLAG_C) != 0;

        switch (inst->cond) {
        case CT_NZ: should_jump = !z; break;
        case CT_Z:  should_jump = z; break;
        case CT_NC: should_jump = !c; break;
        case CT_C:  should_jump = c; break;
        default: break;
        }
    }

    if (should_jump) {
        switch (inst->mode) {
        case AM_D16:
            addr = read16(ctx, ctx->regs.pc);
            ctx->regs.pc = addr;
            return 16;
        case AM_R:
            addr = cpu_read_reg16(ctx, inst->reg_1);
            ctx->regs.pc = addr;
            return 4;
        default:
            printf("Invalid JP mode\n");
            return -1;
        }
    }
    else {
        ctx->regs.pc += (inst->mode == AM_D16) ? 2 : 0;
        return 12; // Conditional jump not taken
    }
}

static int execute_call(cpu_context* ctx, instruction* inst) {
    uint16_t addr = read16(ctx, ctx->regs.pc);
    ctx->regs.pc += 2;

    // Push return address onto stack
    ctx->regs.sp -= 2;
    write16(ctx, ctx->regs.sp, ctx->regs.pc);

    ctx->regs.pc = addr;
    return 24;
}





static int execute_pop(cpu_context* ctx, instruction* inst) {
    uint16_t val = read16(ctx, ctx->regs.sp);
    ctx->regs.sp += 2;
    cpu_write_reg16(ctx, inst->reg_1, val);

    // Special handling for POP AF
    if (inst->reg_1 == RT_AF) {
        ctx->regs.f &= 0xF0; // Lower nibble always 0
    }
    return 12;
}

// Updated register access functions
// Updated register access functions with proper HL handling
// Updated register access functions
const char* reg_type_to_str(reg_type reg) {
    switch (reg) {
    case RT_NONE: return "None";
    case RT_A: return "A";
    case RT_F: return "F";
    case RT_B: return "B";
    case RT_C: return "C";
    case RT_D: return "D";
    case RT_E: return "E";
    case RT_H: return "H";
    case RT_L: return "L";
    case RT_AF: return "AF";
    case RT_BC: return "BC";
    case RT_DE: return "DE";
    case RT_HL: return "HL";
    case RT_SP: return "SP";
    case RT_PC: return "PC";
    default: return "Unknown";
    }
}



// Enhanced register access functions with strict validation
// In cpu.c
uint8_t cpu_read_reg8(cpu_context* ctx, reg_type reg) {
    switch (reg) {
    case RT_A: return ctx->regs.a;
    case RT_F: return ctx->regs.f;
    case RT_B: return ctx->regs.b;
    case RT_C: return ctx->regs.c;
    case RT_D: return ctx->regs.d;
    case RT_E: return ctx->regs.e;
    case RT_H: return ctx->regs.h;
    case RT_L: return ctx->regs.l;
    case RT_HL: {
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        return bus_read(addr);
    }
    default:
        printf("FATAL: Invalid 8-bit read reg %d at PC=%04X\n", reg, ctx->regs.pc);
        return 0xFF;
    }
}

void cpu_write_reg8(cpu_context* ctx, reg_type reg, uint8_t val) {
    switch (reg) {
    case RT_A: ctx->regs.a = val; break;
    case RT_F: ctx->regs.f = val & 0xF0; break;
    case RT_B: ctx->regs.b = val; break;
    case RT_C: ctx->regs.c = val; break;
    case RT_D: ctx->regs.d = val; break;
    case RT_E: ctx->regs.e = val; break;
    case RT_H: ctx->regs.h = val; break;
    case RT_L: ctx->regs.l = val; break;
    case RT_HL: {
        uint16_t addr = cpu_read_reg16(ctx, RT_HL);
        bus_write(addr, val);
        break;
    }
    default:
        printf("FATAL: Invalid 8-bit write reg %d at PC=%04X\n", reg, ctx->regs.pc);
        break;
    }
}




// 16-bit register access functions
uint16_t cpu_read_reg16(cpu_context* ctx, reg_type reg) {
    switch (reg) {
    case RT_AF: return (ctx->regs.a << 8) | ctx->regs.f;
    case RT_BC: return (ctx->regs.b << 8) | ctx->regs.c;
    case RT_DE: return (ctx->regs.d << 8) | ctx->regs.e;
    case RT_HL: return (ctx->regs.h << 8) | ctx->regs.l;
    case RT_SP: return ctx->regs.sp;
    case RT_PC: return ctx->regs.pc;
    default:
        printf("Invalid 16-bit register access: %d\n", reg);
        return 0;
    }
}

void cpu_write_reg16(cpu_context* ctx, reg_type reg, uint16_t val) {
    switch (reg) {
    case RT_AF:
        ctx->regs.a = val >> 8;
        ctx->regs.f = val & 0xF0; // Only upper 4 bits for flags
        break;
    case RT_BC:
        ctx->regs.b = val >> 8;
        ctx->regs.c = val & 0xFF;
        break;
    case RT_DE:
        ctx->regs.d = val >> 8;
        ctx->regs.e = val & 0xFF;
        break;
    case RT_HL:
        ctx->regs.h = val >> 8;
        ctx->regs.l = val & 0xFF;
        break;
    case RT_SP:
        ctx->regs.sp = val;
        break;
    case RT_PC:
        ctx->regs.pc = val;
        break;
    default:
        printf("Invalid 16-bit register write: %d\n", reg);
    }
}