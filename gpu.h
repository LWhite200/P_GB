#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t mode;       // Current GPU mode (0-3)
    uint8_t line;       // Current scanline (0-153)
    uint16_t ticks;     // Cycle counter for current mode
    uint32_t framebuffer[160 * 144]; // ARGB pixels
} gpu_context;

// Function declarations
void gpu_init(void);
void gpu_step(int cycles);
gpu_context* gpu_get_context(void);
bool gpu_in_vblank(void);

#endif