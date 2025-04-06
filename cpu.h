#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "instructions.h"
#include "cb_instructions.h"

// CPU Flag Definitions
#define FLAG_Z 0x80  // Zero flag
#define FLAG_N 0x40  // Subtract flag
#define FLAG_H 0x20  // Half Carry flag
#define FLAG_C 0x10  // Carry flag

typedef struct {
    uint16_t pc;
    uint16_t sp;

    union {
        struct {
            uint8_t f;  // Flags register (uses upper 4 bits)
            uint8_t a;  // Accumulator
        };
        uint16_t af;
    };

    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };
} cpu_registers;

typedef struct cpu_context {
    cpu_registers regs;
    uint16_t fetched_data;
    uint8_t opcode;
    void* cur_inst;

    // Memory access tracking
    uint16_t mem_dest;
    bool dest_is_mem;

    // Current execution state
    bool halted;
    bool interrupts_enabled;
    bool stopped;
    bool ime;
    int cycles;

    struct {
        uint64_t cycles;  // Total cycles executed
    } clock;

    uint8_t int_enable;    // FFFF - IE register
    uint8_t int_flags;     // FF0F - IF register
    bool interrupts_master_enable;
} cpu_context;

// Register access functions
uint8_t cpu_read_reg8(cpu_context* ctx, reg_type reg);
void cpu_write_reg8(cpu_context* ctx, reg_type reg, uint8_t val);
uint16_t cpu_read_reg16(cpu_context* ctx, reg_type reg);
const char* reg_type_to_str(reg_type reg);
void cpu_write_reg16(cpu_context* ctx, reg_type reg, uint16_t val);

// CPU operations
void cpu_init(cpu_context* ctx);
int cpu_step(cpu_context* ctx);

// Flag manipulation helpers
static inline void set_flag(cpu_context* ctx, uint8_t flag) {
    ctx->regs.f |= flag;
}

static inline void clear_flag(cpu_context* ctx, uint8_t flag) {
    ctx->regs.f &= ~flag;
}

static inline bool check_flag(cpu_context* ctx, uint8_t flag) {
    return (ctx->regs.f & flag) != 0;
}



#endif // CPU_H