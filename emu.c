#include <stdio.h>
#include <stdlib.h>
#include "cart.h"

int emu_run(int argc, char** argv) {
    // Change this to the actual file you want to read
    const char* rom_path = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\tetris.gb";

    size_t rom_size;
    unsigned char* rom = load_rom(rom_path, &rom_size);
    if (!rom) {
        printf("Failed to load ROM at: %s\n", rom_path);
        return 1;
    }

    printf("ROM loaded: %s (%zu bytes)\n\n", rom_path, rom_size);

    for (size_t pc = 0x0100; pc < rom_size; ++pc) {
        printf("0x%04zX: 0x%02X\n", pc, rom[pc]);
    }

    free(rom);
    return 0;
}
