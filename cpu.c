#include "cpu.h"
#include "mmu.h"
#include <stdio.h>

void cpu_init(CPU* cpu) {
    cpu->pc = 0x100;
    cpu->sp = 0xFFFE;
    cpu->a = cpu->f = cpu->b = cpu->c = cpu->d = cpu->e = cpu->h = cpu->l = 0;
}

void cpu_step(CPU* cpu) {
    uint8_t opcode = mmu_read(cpu->pc++);

    // Debug print (place this at the start of cpu_step)
    printf("PC: 0x%04X | A: 0x%02X | BC: 0x%02X%02X | Op: 0x%02X\n",
        cpu->pc - 1, cpu->a, cpu->b, cpu->c, opcode);

    switch (opcode) {
    case 0x00: // RLC B
    {
        uint8_t val = cpu->b;
        // TODO: Implement RLC operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x01: // RLC C
    {
        uint8_t val = cpu->c;
        // TODO: Implement RLC operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x02: // RLC D
    {
        uint8_t val = cpu->d;
        // TODO: Implement RLC operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x03: // RLC E
    {
        uint8_t val = cpu->e;
        // TODO: Implement RLC operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x04: // RLC H
    {
        uint8_t val = cpu->h;
        // TODO: Implement RLC operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x05: // RLC L
    {
        uint8_t val = cpu->l;
        // TODO: Implement RLC operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x06: // RLC (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement RLC operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x07: // RLC A
    {
        uint8_t val = cpu->a;
        // TODO: Implement RLC operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x08: // RRC B
    {
        uint8_t val = cpu->b;
        // TODO: Implement RRC operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x09: // RRC C
    {
        uint8_t val = cpu->c;
        // TODO: Implement RRC operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x0A: // RRC D
    {
        uint8_t val = cpu->d;
        // TODO: Implement RRC operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x0B: // RRC E
    {
        uint8_t val = cpu->e;
        // TODO: Implement RRC operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x0C: // RRC H
    {
        uint8_t val = cpu->h;
        // TODO: Implement RRC operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x0D: // RRC L
    {
        uint8_t val = cpu->l;
        // TODO: Implement RRC operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x0E: // RRC (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement RRC operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x0F: // RRC A
    {
        uint8_t val = cpu->a;
        // TODO: Implement RRC operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x10: // RL B
    {
        uint8_t val = cpu->b;
        // TODO: Implement RL operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x11: // RL C
    {
        uint8_t val = cpu->c;
        // TODO: Implement RL operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x12: // RL D
    {
        uint8_t val = cpu->d;
        // TODO: Implement RL operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x13: // RL E
    {
        uint8_t val = cpu->e;
        // TODO: Implement RL operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x14: // RL H
    {
        uint8_t val = cpu->h;
        // TODO: Implement RL operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x15: // RL L
    {
        uint8_t val = cpu->l;
        // TODO: Implement RL operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x16: // RL (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement RL operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x17: // RL A
    {
        uint8_t val = cpu->a;
        // TODO: Implement RL operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x18: // RR B
    {
        uint8_t val = cpu->b;
        // TODO: Implement RR operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x19: // RR C
    {
        uint8_t val = cpu->c;
        // TODO: Implement RR operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x1A: // RR D
    {
        uint8_t val = cpu->d;
        // TODO: Implement RR operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x1B: // RR E
    {
        uint8_t val = cpu->e;
        // TODO: Implement RR operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x1C: // RR H
    {
        uint8_t val = cpu->h;
        // TODO: Implement RR operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x1D: // RR L
    {
        uint8_t val = cpu->l;
        // TODO: Implement RR operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x1E: // RR (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement RR operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x1F: // RR A
    {
        uint8_t val = cpu->a;
        // TODO: Implement RR operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x20: // SLA B
    {
        uint8_t val = cpu->b;
        // TODO: Implement SLA operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x21: // SLA C
    {
        uint8_t val = cpu->c;
        // TODO: Implement SLA operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x22: // SLA D
    {
        uint8_t val = cpu->d;
        // TODO: Implement SLA operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x23: // SLA E
    {
        uint8_t val = cpu->e;
        // TODO: Implement SLA operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x24: // SLA H
    {
        uint8_t val = cpu->h;
        // TODO: Implement SLA operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x25: // SLA L
    {
        uint8_t val = cpu->l;
        // TODO: Implement SLA operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x26: // SLA (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement SLA operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x27: // SLA A
    {
        uint8_t val = cpu->a;
        // TODO: Implement SLA operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x28: // SRA B
    {
        uint8_t val = cpu->b;
        // TODO: Implement SRA operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x29: // SRA C
    {
        uint8_t val = cpu->c;
        // TODO: Implement SRA operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x2A: // SRA D
    {
        uint8_t val = cpu->d;
        // TODO: Implement SRA operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x2B: // SRA E
    {
        uint8_t val = cpu->e;
        // TODO: Implement SRA operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x2C: // SRA H
    {
        uint8_t val = cpu->h;
        // TODO: Implement SRA operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x2D: // SRA L
    {
        uint8_t val = cpu->l;
        // TODO: Implement SRA operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x2E: // SRA (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement SRA operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x2F: // SRA A
    {
        uint8_t val = cpu->a;
        // TODO: Implement SRA operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x30: // SWAP B
        // TODO: Implement SWAP B
        break;

    case 0x31: // SWAP C
        // TODO: Implement SWAP C
        break;

    case 0x32: // SWAP D
        // TODO: Implement SWAP D
        break;

    case 0x33: // SWAP E
        // TODO: Implement SWAP E
        break;

    case 0x34: // SWAP H
        // TODO: Implement SWAP H
        break;

    case 0x35: // SWAP L
        // TODO: Implement SWAP L
        break;

    case 0x36: // SWAP (HL)
        // TODO: Implement SWAP (HL)
        break;

    case 0x37: // SWAP A
        // TODO: Implement SWAP A
        break;

    case 0x38: // SRL B
    {
        uint8_t val = cpu->b;
        // TODO: Implement SRL operation
        cpu->b = val;
        // TODO: Set flags
    }
    break;

    case 0x39: // SRL C
    {
        uint8_t val = cpu->c;
        // TODO: Implement SRL operation
        cpu->c = val;
        // TODO: Set flags
    }
    break;

    case 0x3A: // SRL D
    {
        uint8_t val = cpu->d;
        // TODO: Implement SRL operation
        cpu->d = val;
        // TODO: Set flags
    }
    break;

    case 0x3B: // SRL E
    {
        uint8_t val = cpu->e;
        // TODO: Implement SRL operation
        cpu->e = val;
        // TODO: Set flags
    }
    break;

    case 0x3C: // SRL H
    {
        uint8_t val = cpu->h;
        // TODO: Implement SRL operation
        cpu->h = val;
        // TODO: Set flags
    }
    break;

    case 0x3D: // SRL L
    {
        uint8_t val = cpu->l;
        // TODO: Implement SRL operation
        cpu->l = val;
        // TODO: Set flags
    }
    break;

    case 0x3E: // SRL (HL)
    {
        uint16_t addr = (cpu->h << 8) | cpu->l;
        uint8_t val = mmu_read(addr);
        // TODO: Implement SRL operation
        mmu_write(addr, val);
        // TODO: Set flags
    }
    break;

    case 0x3F: // SRL A
    {
        uint8_t val = cpu->a;
        // TODO: Implement SRL operation
        cpu->a = val;
        // TODO: Set flags
    }
    break;

    case 0x40: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x41: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x42: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x43: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x44: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x45: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x46: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x47: // BIT 0
        // TODO: Implement BIT 0
        break;

    case 0x48: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x49: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4A: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4B: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4C: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4D: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4E: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x4F: // BIT 1
        // TODO: Implement BIT 1
        break;

    case 0x50: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x51: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x52: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x53: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x54: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x55: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x56: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x57: // BIT 2
        // TODO: Implement BIT 2
        break;

    case 0x58: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x59: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5A: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5B: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5C: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5D: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5E: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x5F: // BIT 3
        // TODO: Implement BIT 3
        break;

    case 0x60: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x61: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x62: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x63: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x64: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x65: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x66: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x67: // BIT 4
        // TODO: Implement BIT 4
        break;

    case 0x68: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x69: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6A: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6B: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6C: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6D: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6E: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x6F: // BIT 5
        // TODO: Implement BIT 5
        break;

    case 0x70: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x71: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x72: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x73: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x74: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x75: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x76: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x77: // BIT 6
        // TODO: Implement BIT 6
        break;

    case 0x78: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x79: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7A: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7B: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7C: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7D: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7E: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x7F: // BIT 7
        // TODO: Implement BIT 7
        break;

    case 0x80: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x81: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x82: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x83: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x84: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x85: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x86: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x87: // RES 0
        // TODO: Implement RES 0
        break;

    case 0x88: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x89: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8A: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8B: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8C: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8D: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8E: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x8F: // RES 1
        // TODO: Implement RES 1
        break;

    case 0x90: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x91: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x92: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x93: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x94: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x95: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x96: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x97: // RES 2
        // TODO: Implement RES 2
        break;

    case 0x98: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x99: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9A: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9B: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9C: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9D: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9E: // RES 3
        // TODO: Implement RES 3
        break;

    case 0x9F: // RES 3
        // TODO: Implement RES 3
        break;

    case 0xA0: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA1: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA2: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA3: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA4: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA5: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA6: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA7: // RES 4
        // TODO: Implement RES 4
        break;

    case 0xA8: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xA9: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAA: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAB: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAC: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAD: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAE: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xAF: // RES 5
        // TODO: Implement RES 5
        break;

    case 0xB0: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB1: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB2: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB3: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB4: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB5: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB6: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB7: // RES 6
        // TODO: Implement RES 6
        break;

    case 0xB8: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xB9: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBA: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBB: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBC: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBD: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBE: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xBF: // RES 7
        // TODO: Implement RES 7
        break;

    case 0xC0: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC1: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC2: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC3: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC4: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC5: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC6: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC7: // SET 0
        // TODO: Implement SET 0
        break;

    case 0xC8: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xC9: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCA: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCB: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCC: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCD: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCE: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xCF: // SET 1
        // TODO: Implement SET 1
        break;

    case 0xD0: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD1: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD2: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD3: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD4: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD5: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD6: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD7: // SET 2
        // TODO: Implement SET 2
        break;

    case 0xD8: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xD9: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDA: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDB: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDC: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDD: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDE: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xDF: // SET 3
        // TODO: Implement SET 3
        break;

    case 0xE0: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE1: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE2: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE3: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE4: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE5: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE6: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE7: // SET 4
        // TODO: Implement SET 4
        break;

    case 0xE8: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xE9: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xEA: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xEB: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xEC: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xED: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xEE: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xEF: // SET 5
        // TODO: Implement SET 5
        break;

    case 0xF0: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF1: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF2: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF3: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF4: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF5: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF6: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF7: // SET 6
        // TODO: Implement SET 6
        break;

    case 0xF8: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xF9: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFA: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFB: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFC: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFD: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFE: // SET 7
        // TODO: Implement SET 7
        break;

    case 0xFF: // SET 7
        // TODO: Implement SET 7
        break;

    default:
        printf("Unknown CB-prefixed opcode: 0x%02X\n", opcode);
        break;
    }
}