#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "instructions.h"
#include "cpu.h"
#include "bus.h"

const char* rt_lookup[] = {
    "<NONE>", "A", "F", "B", "C", "D", "E", "H",
    "L", "AF", "BC", "DE", "HL", "SP", "PC"
};

int inst_size(const instruction* inst) {
    if (!inst) return 1;

    switch (inst->mode) {
    case AM_IMP:    return 1;
    case AM_R:      return 1;
    case AM_R_R:    return 1;
    case AM_MR_R:   return 1;
    case AM_R_MR:   return 1;
    case AM_MR:     return 1;
    case AM_R_D8:   return 2;
    case AM_D8:     return 2;
    case AM_A8_R:   return 2;
    case AM_R_A8:   return 2;
    case AM_R_D16:  return 3;
    case AM_D16:    return 3;
    case AM_A16_R:  return 3;
    case AM_R_HLI:  return 1;
    case AM_R_HLD:  return 1;
    case AM_HLI_R:  return 1;
    case AM_HLD_R:  return 1;
    case AM_HL_SPR: return 2;
    case AM_MR_D8:  return 2;
    case AM_R_A16:  return 3;
    default:        return 1;
    }
}

void print_usage(const char* program_name) {
    printf("GameBoy ROM Disassembler\n");
    printf("Usage: %s <rom_file.gb>\n\n", program_name);
    printf("Example:\n  %s \"roms/tetris.gb\"\n", program_name);
}

const char* get_operand_format(in_type type, addr_mode mode) {
    if (type == IN_LDH) return "FF00+%02Xh";
    return "%02Xh";
}

int main(int argc, char* argv[]) {
    // Handle command line arguments
    const char* rom_file_path = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\tetris.gb";

    FILE* rom_file = fopen(rom_file_path, "rb");
    if (!rom_file) {
        printf("Error: Could not open ROM file: %s\n", rom_file_path);
        return 1;
    }

    // Read ROM file
    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    fseek(rom_file, 0, SEEK_SET);

    if (rom_size <= 0) {
        printf("Error: Invalid ROM file size\n");
        fclose(rom_file);
        return 1;
    }

    uint8_t* rom_data = malloc(rom_size);
    if (!rom_data || fread(rom_data, 1, rom_size, rom_file) != rom_size) {
        printf("Error: Failed to read ROM data\n");
        if (rom_data) free(rom_data);
        fclose(rom_file);
        return 1;
    }
    fclose(rom_file);

    printf("ROM: %s (%ld bytes)\n", rom_file_path, rom_size);
    printf("Disassembly:\n\n");

    cpu_context ctx = { 0 };
    long pc = 0;
    int count = 0;

    // Header with fixed-width columns
    printf("%-8s %-8s %-22s %-6s\n", "Addr", "Opcode", "Instruction", "Bytes");
    printf("-------- -------- ---------------------- ------\n");

    while (pc < rom_size) {
        uint8_t opcode = rom_data[pc];
        instruction* inst = instruction_by_opcode(opcode);

        // Print address and opcode
        printf("%-8lX %-8X ", pc, opcode);

        if (!inst || inst->type == IN_ERR) {
            printf("DB %02Xh                ; Unknown\n", opcode);
            pc++;
            count++;
            continue;
        }

        // Setup context
        ctx.cur_inst = inst;
        ctx.regs.pc = pc;
        ctx.opcode = opcode;

        // Handle instruction size and operands
        int size = inst_size(inst);
        if (size > 1 && pc + size <= rom_size) {
            if (size == 2) {
                ctx.fetched_data = rom_data[pc + 1];
            }
            else if (size == 3) {
                ctx.fetched_data = (rom_data[pc + 2] << 8) | rom_data[pc + 1];
            }
        }

        // Print instruction mnemonic
        printf("%-22s ", inst_name(inst->type));

        // Print operands based on addressing mode
        switch (inst->mode) {
        case AM_IMP: break;

        case AM_R:
            printf("%s", rt_lookup[inst->reg_1]);
            break;

        case AM_R_R:
            printf("%s, %s", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_R_D8:
            printf("%s, %02Xh", rt_lookup[inst->reg_1], ctx.fetched_data);
            break;

        case AM_D8:
            printf("%s", get_operand_format(inst->type, inst->mode), ctx.fetched_data);
            break;

        case AM_R_D16:
            printf("%s, %04Xh", rt_lookup[inst->reg_1], ctx.fetched_data);
            break;

        case AM_D16:
            printf("%04Xh", ctx.fetched_data);
            break;

        case AM_A8_R:
            printf("(%s), %s", inst->type == IN_LDH ? "FF00+" : "", rt_lookup[inst->reg_2]);
            if (inst->type == IN_LDH) printf("%02Xh", ctx.fetched_data);
            break;

        case AM_R_A8:
            printf("%s, (%s%s", rt_lookup[inst->reg_1], inst->type == IN_LDH ? "FF00+" : "", inst->type == IN_LDH ? ")" : "");
            if (inst->type == IN_LDH) printf("%02Xh)", ctx.fetched_data);
            break;

        case AM_MR_R:
            printf("(%s), %s", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_R_MR:
            printf("%s, (%s)", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_MR:
            printf("(%s)", rt_lookup[inst->reg_1]);
            break;

        case AM_HLI_R:
            printf("(%s+), %s", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_HLD_R:
            printf("(%s-), %s", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_R_HLI:
            printf("%s, (%s+)", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_R_HLD:
            printf("%s, (%s-)", rt_lookup[inst->reg_1], rt_lookup[inst->reg_2]);
            break;

        case AM_HL_SPR: {
            int8_t offset = (int8_t)ctx.fetched_data;
            printf("HL, SP%+d (%+02Xh)", offset, ctx.fetched_data);
            break;
        }

        case AM_MR_D8:
            printf("(%s), %02Xh", rt_lookup[inst->reg_1], ctx.fetched_data);
            break;

        case AM_A16_R:
            printf("(%04Xh), %s", ctx.fetched_data, rt_lookup[inst->reg_2]);
            break;

        case AM_R_A16:
            printf("%s, (%04Xh)", rt_lookup[inst->reg_1], ctx.fetched_data);
            break;

        default:
            printf("; Unhandled mode %d", inst->mode);
        }

        // Show raw bytes
        printf(" %-6s", ""); // Padding to ensure column alignment
        for (int i = 0; i < size && pc + i < rom_size; i++) {
            printf("%02X ", rom_data[pc + i]);
        }
        printf("\n");

        pc += size;
        count++;
    }

    printf("\nFinished. Processed %d instructions.\n", count);
    free(rom_data);
    return 0;
}
