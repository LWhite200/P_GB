#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "cpu.h"
#include "gpu.h"
#include "bus.h"
#include <stdio.h>

// Hardcoded ROM path
static const char* ROM_PATH = "C:\\Users\\TriBlackInferno\\Documents\\Piracy\\gba\\Tetris.gb";

cpu_context ctx;

/* Emulator rendering state */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* screen_texture = NULL;

/* Loads the ROM into the emulator */
static bool load_rom(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        SDL_Log("Failed to open ROM file: %s", path);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Game Boy ROMs can be up to 8MB (though early ones are 32KB-1MB)
    if (size > 8 * 1024 * 1024) {
        SDL_Log("ROM too large: %ld bytes", size);
        fclose(file);
        return false;
    }

    // Load ROM to cartridge space (0x0000-0x7FFF for 32KB ROMs)
    size_t read = fread(&memory[0x0000], 1, size, file);
    fclose(file);

    SDL_Log("Loaded %zu bytes of ROM from %s", read, path);

    // Print Nintendo logo from ROM header (should match actual GB logo)
    SDL_Log("Nintendo Logo in ROM:");
    for (int i = 0x0104; i < 0x0134; i++) {
        if ((i - 0x0104) % 16 == 0) printf("\n");
        printf("%02X ", memory[i]);
    }
    printf("\n");

    return true;
}

/* Debug GPU state */
static void debug_gpu_state() {
    gpu_context* gpu = gpu_get_context();
    SDL_Log("GPU State - Mode: %d, Line: %d, Ticks: %u",
        gpu->mode, gpu->line, gpu->ticks);

    // Check LCD control register
    uint8_t lcdc = bus_read(0xFF40);
    SDL_Log("LCDC: %02X (Display %s, BG %s, Window %s, Sprites %s)",
        lcdc,
        (lcdc & 0x80) ? "ON" : "OFF",
        (lcdc & 0x01) ? "ON" : "OFF",
        (lcdc & 0x20) ? "ON" : "OFF",
        (lcdc & 0x02) ? "ON" : "OFF");
}

/* This function runs once at startup */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_SetAppMetadata("GameBoy Emulator", "1.0", "com.example.gameboy-emulator");

    // Initialize SDL exactly as you had it
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());  // Corrected condition here
        return SDL_APP_FAILURE;
    }

    // Create 4x scaled window (160x144 -> 640x576)
    window = SDL_CreateWindow("GameBoy Emulator", 640, 576, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Set renderer scale to maintain pixel-perfect scaling
    SDL_SetRenderScale(renderer, 4.0f, 4.0f);

    // Create texture with explicit format specification
    screen_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        160, 144);
    if (!screen_texture) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize components
    gpu_init();
    cpu_init(&ctx);

    if (!load_rom(ROM_PATH)) {
        SDL_Log("ROM loading failed - using test pattern");
        // Fill memory with test pattern if ROM fails
        for (int i = 0; i < 0x8000; i++) {
            memory[i] = i % 256;
        }
    }

    // Debug initial state
    debug_gpu_state();
    SDL_Log("Initial CPU PC: %04X", ctx.regs.pc);

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

/* Main emulation frame with debug rendering */
// Include your required headers (e.g., SDL, CPU, GPU, etc.)

// Assuming the context of `gpu_context` is provided correctly through gpu_get_context()
SDL_AppResult SDL_AppIterate(void* appstate) {
    static int frame_count = 0;
    const int CYCLES_PER_FRAME = 70224;
    int cycles_this_frame = 0;

    // Debug CPU stuck detection
    static uint16_t last_pc = 0xFFFF;
    static int pc_repeat_count = 0;

    // Run one frame's worth of cycles
    while (cycles_this_frame < CYCLES_PER_FRAME) {
        int cycles = cpu_step(&ctx);
        gpu_step(cycles);
        cycles_this_frame += cycles;

        // Detect if CPU is stuck
        if (ctx.regs.pc == last_pc) {
            pc_repeat_count++;
            if (pc_repeat_count > 1000) {
                SDL_Log("CPU stuck at PC=%04X, breaking out", ctx.regs.pc);
                ctx.regs.pc++; // Force advance (temporary)
                pc_repeat_count = 0;
            }
        }
        else {
            pc_repeat_count = 0;
            last_pc = ctx.regs.pc;
        }

        // Debug: Print CPU state occasionally
        if (frame_count % 60 == 0 && cycles_this_frame < 1000) {
            SDL_Log("PC:%04X OP:%02X A:%02X F:%02X BC:%04X DE:%04X HL:%04X",
                ctx.regs.pc, ctx.opcode,
                ctx.regs.a, ctx.regs.f,
                (ctx.regs.b << 8) | ctx.regs.c,
                (ctx.regs.d << 8) | ctx.regs.e,
                (ctx.regs.h << 8) | ctx.regs.l);
        }
    }

    // Attempt to render
    void* pixels;
    int pitch;
    int lock_result = SDL_LockTexture(screen_texture, NULL, &pixels, &pitch);

    if (lock_result == 0) {
        gpu_context* gpu = gpu_get_context();

        // Check if the GPU context and framebuffer are valid
        if (!gpu || !gpu->framebuffer) {
            SDL_Log("GPU context or framebuffer not initialized properly!");
            SDL_UnlockTexture(screen_texture); // Make sure to unlock the texture
            return SDL_APP_FAILURE;  // Return failure if we can't render
        }

        // Debug: Log framebuffer address
        SDL_Log("Framebuffer address: %p", gpu->framebuffer);

        // Copy framebuffer to texture
        memcpy(pixels, gpu->framebuffer, 160 * 144 * sizeof(uint32_t));
        SDL_UnlockTexture(screen_texture);
    }
    else {
        SDL_Log("Texture lock failed (frame %d): %s", frame_count, SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Render the texture to the window
    SDL_RenderClear(renderer);
    if (SDL_RenderTexture(renderer, screen_texture, NULL, NULL) != 0) {
        SDL_Log("RenderTexture failed: %s", SDL_GetError());
    }
    SDL_RenderPresent(renderer);

    frame_count++;
    if (frame_count % 60 == 0) {
        debug_gpu_state();
    }

    // Frame rate limiting
    static uint32_t last_frame_time = 0;
    uint32_t current_time = SDL_GetTicks();
    uint32_t frame_time = current_time - last_frame_time;

    if (frame_time < 16) { // ~60fps
        SDL_Delay(16 - frame_time);
    }
    last_frame_time = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}








void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (screen_texture) SDL_DestroyTexture(screen_texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}