#include "gpu.h"
#include "bus.h"
#include <string.h>
#include <stdio.h>

static gpu_context gpu;  // Declare a static gpu_context object

// Debug colors for GPU modes
static const uint32_t MODE_COLORS[4] = {
    0xFF00FF00, // HBlank - Green
    0xFF0000FF, // VBlank - Blue
    0xFFFF0000, // OAM    - Red
    0xFFFFFF00  // VRAM   - Yellow
};

// Forward declaration of internal function
static void render_scanline(void);

void gpu_init(void) {
    memset(&gpu, 0, sizeof(gpu));
    gpu.mode = 2; // Start in OAM search mode

    // Fill with white background
    for (int i = 0; i < 160 * 144; i++) {
        gpu.framebuffer[i] = 0xFFFFFFFF;
    }

    // Default LCD registers
    bus_write(0xFF40, 0x91); // LCDC: Display on, BG on, tiles at 8000
    bus_write(0xFF42, 0x00); // SCY
    bus_write(0xFF43, 0x00); // SCX
    bus_write(0xFF47, 0xFC); // BGP: Default palette (11 10 01 00)
}

void gpu_step(int cycles) {
    gpu.ticks += cycles;

    switch (gpu.mode) {
    case 2: // OAM Search (80 cycles)
        if (gpu.ticks >= 80) {
            gpu.ticks -= 80;
            gpu.mode = 3;
        }
        break;

    case 3: // Pixel Transfer (172 cycles)
        if (gpu.ticks >= 172) {
            gpu.ticks -= 172;
            render_scanline();
            gpu.mode = 0;
        }
        break;

    case 0: // HBlank (204 cycles)
        if (gpu.ticks >= 204) {
            gpu.ticks -= 204;
            gpu.line++;

            if (gpu.line == 144) {
                gpu.mode = 1; // Enter VBlank
            }
            else {
                gpu.mode = 2; // Next line
            }
        }
        break;

    case 1: // VBlank (4560 cycles total)
        if (gpu.ticks >= 456) {
            gpu.ticks -= 456;
            gpu.line++;

            if (gpu.line > 153) {
                gpu.line = 0;
                gpu.mode = 2;
            }
        }
        break;
    }

    // Update LY register
    bus_write(0xFF44, gpu.line);

    // Set mode flags in STAT register
    uint8_t stat = bus_read(0xFF41);
    stat = (stat & 0xFC) | gpu.mode;
    bus_write(0xFF41, stat);
}

static void render_scanline(void) {
    uint8_t lcdc = bus_read(0xFF40);

    // Debug: Show mode colors instead of actual rendering
    if (0) { // Change to 1 for debug view
        uint32_t color = MODE_COLORS[gpu.mode];
        for (int x = 0; x < 160; x++) {
            gpu.framebuffer[gpu.line * 160 + x] = color;
        }
        return;
    }

    // Only render if display is enabled
    if (!(lcdc & 0x80)) {
        return;
    }

    uint8_t scroll_y = bus_read(0xFF42);
    uint8_t scroll_x = bus_read(0xFF43);
    uint8_t bg_palette = bus_read(0xFF47);

    uint16_t tilemap = (lcdc & 0x08) ? 0x9C00 : 0x9800;
    bool signed_tiles = !(lcdc & 0x10);

    for (int x = 0; x < 160; x++) {
        uint8_t pixel_y = (gpu.line + scroll_y) % 256;
        uint8_t pixel_x = (x + scroll_x) % 256;

        // Get tile number
        uint16_t tile_addr = tilemap + ((pixel_y / 8) * 32 + (pixel_x / 8));
        int8_t tile_num = signed_tiles ? (int8_t)bus_read(tile_addr) : bus_read(tile_addr);

        // Get tile data address
        uint16_t tile_data_addr;
        if (signed_tiles) {
            tile_data_addr = 0x9000 + ((int8_t)tile_num * 16);
        }
        else {
            tile_data_addr = 0x8000 + (tile_num * 16);
        }

        // Get pixel line within tile (0-7)
        uint8_t line = pixel_y % 8;
        uint8_t data1 = bus_read(tile_data_addr + line * 2);
        uint8_t data2 = bus_read(tile_data_addr + line * 2 + 1);

        // Get pixel bit (0-7, right to left)
        uint8_t bit = 7 - (pixel_x % 8);
        uint8_t color_num = ((data2 >> bit) & 1) << 1 | ((data1 >> bit) & 1);
        uint8_t color = (bg_palette >> (color_num * 2)) & 0x03;

        // Convert to ARGB (Game Boy colors are 00, 55, AA, FF)
        uint32_t rgb;
        switch (color) {
        case 0: rgb = 0x00FFFFFF; break; // White
        case 1: rgb = 0x00AAAAAA; break;  // Light gray
        case 2: rgb = 0x00555555; break;  // Dark gray
        case 3: rgb = 0x00000000; break;  // Black
        default: rgb = 0x00FF00FF;        // Magenta (error)
        }

        gpu.framebuffer[gpu.line * 160 + x] = rgb | 0xFF000000;
    }
}

gpu_context* gpu_get_context(void) {
    return &gpu;
}

bool gpu_in_vblank(void) {
    return gpu.mode == 1;
}
