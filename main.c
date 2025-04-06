#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "cpu.h"
#include "gpu.h"
#include "bus.h"

// Hardcoded ROM path (adjust to your actual ROM location)
char* fixed_rom_path = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\tetris.gb";

void load_rom(const char* path, uint16_t load_addr) {
    // Hardcoded ROM path (use the correct path to your ROM)
    const char* fixed_rom_path = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\tetris.gb";  // Change this to the actual ROM path
    // const char* fixed_rom_path = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\PokemonBlue.gb";

    FILE* file = fopen(fixed_rom_path, "rb");
    if (!file) {
        printf("Failed to open ROM file\n");
        return;
    }

    dump_memory(0x0000, 256);  // Dump first 256 bytes
    dump_memory(0x0200, 256);  // Dump around the jump target

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size > 0x10000 - load_addr) {
        printf("ROM too large\n");
        fclose(file);
        return;
    }

    fread(&memory[load_addr], 1, size, file);
    fclose(file);
    printf("Loaded ROM to memory at 0x%04X\n", load_addr);
}

int main(int argc, char* argv[]) {
    // Initialize SDL3
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create window (scaled 4x for visibility)
    SDL_Window* window = SDL_CreateWindow("GameBoy Emulator",
        640, 480,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create texture for GameBoy output (160x144)
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        160, 144);
    if (!texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize emulator components
    gpu_init();
    cpu_context ctx;
    cpu_init(&ctx);

    // Load ROM
    printf("Loading ROM from: %s\n", fixed_rom_path);
    load_rom(fixed_rom_path, 0x0100);  // Standard GameBoy ROM load address

    // Main emulation loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Run one frame (70224 cycles = 1 frame)
        int frame_cycles = 0;
        while (frame_cycles < 70224) {
            int cycles = cpu_step(&ctx);
            gpu_step(cycles);
            frame_cycles += cycles;
        }

        // Update display if in VBlank
        if (gpu_get_context()->mode == 1 && gpu_get_context()->line == 144) {
            // Lock texture for writing
            void* pixels;
            int pitch;
            SDL_LockTexture(texture, NULL, &pixels, &pitch);

            // Copy framebuffer
            memcpy(pixels, gpu_get_context()->framebuffer, 160 * 144 * sizeof(uint32_t));

            SDL_UnlockTexture(texture);

            // Render
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}