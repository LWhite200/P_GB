#include "cpu.h"
#include "mmu.h"
#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("GB Emu", 160, 144, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 0);

    // Initialize CPU and load ROM
    CPU cpu;
    cpu_init(&cpu);
    load_rom("C:/Users/TriBlackInferno/Documents/Piracy/gba/Tetris.gb");

    while (1) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) return 0;
        }

        // Run CPU for a frame
        for (int i = 0; i < 100; i++) cpu_step(&cpu);

        // Render test pattern
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear black
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White dots
        for (int y = 0; y < 144; y += 8) {
            for (int x = 0; x < 160; x += 8) {
                if ((x + y) % 16 == 0) {
                    SDL_RenderPoint(renderer, x, y);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }
}