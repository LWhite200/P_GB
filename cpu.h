#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "common.h"
#include "instructions.h"


typedef struct {
    uint16_t pc;
    uint16_t sp;

    union {
        struct {
            uint8_t f;
            uint8_t a;
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

typedef struct {
    cpu_registers regs;
    uint16_t fetched_data;

    uint8_t opcode;
    void* cur_inst;

    // Add these:
    uint16_t mem_dest;
    bool dest_is_mem;
    uint8_t cur_opcode;
} cpu_context;

#endif // CPU_H