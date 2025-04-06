#include "gpu.h"
#include "bus.h"
#include <string.h>

static gpu_context ctx;

// Forward declaration of internal functions
static void render_scanline(void);

void gpu_init(void) {
    memset(&ctx, 0, sizeof(ctx));
    ctx.framebuffer = malloc(160 * 144 * sizeof(uint32_t));
    ctx.mode = 2; // Start in OAM search mode

    // Initialize VRAM with test pattern (remove in final version)
    for (int i = 0; i < 128; i++) {
        bus_write(0x8000 + i * 16, 0xFF); // First line of tile = solid
        bus_write(0x8000 + i * 16 + 1, 0x00);
    }

    // Default LCD registers
    bus_write(0xFF40, 0x91); // LCDC: Display on, BG on, tiles at 8000
    bus_write(0xFF42, 0x00); // SCY
    bus_write(0xFF43, 0x00); // SCX
    bus_write(0xFF47, 0xFC); // BGP: Default palette
}

void gpu_step(int cycles) {
    ctx.ticks += cycles;

    switch (ctx.mode) {
    case 2: // OAM Search (80 cycles)
        if (ctx.ticks >= 80) {
            ctx.ticks -= 80;
            ctx.mode = 3;
        }
        break;

    case 3: // Pixel Transfer (172 cycles)
        if (ctx.ticks >= 172) {
            ctx.ticks -= 172;

            if (bus_read(0xFF40) & 0x80) {
                render_scanline();
            }

            ctx.mode = 0;
        }
        break;

    case 0: // HBlank (204 cycles)
        if (ctx.ticks >= 204) {
            ctx.ticks -= 204;

            if (++ctx.line == 144) {
                ctx.mode = 1; // Enter VBlank
            }
            else {
                ctx.mode = 2; // Next line
            }
        }
        break;

    case 1: // VBlank (4560 cycles total)
        if (ctx.ticks >= 456) {
            ctx.ticks -= 456;
            if (++ctx.line > 153) {
                ctx.line = 0;
                ctx.mode = 2;
            }
        }
        break;
    }

    bus_write(0xFF44, ctx.line);
}

static void render_scanline(void) {
    uint8_t lcdc = bus_read(0xFF40);
    uint8_t scroll_y = bus_read(0xFF42);
    uint8_t scroll_x = bus_read(0xFF43);
    uint8_t bg_palette = bus_read(0xFF47);

    uint16_t tilemap = (lcdc & 0x08) ? 0x9C00 : 0x9800;
    bool signed_tiles = !(lcdc & 0x10);

    for (int x = 0; x < 160; x++) {
        uint8_t pixel_y = (ctx.line + scroll_y) % 256;
        uint8_t pixel_x = (x + scroll_x) % 256;

        uint16_t tile_addr = tilemap + ((pixel_y / 8) * 32 + (pixel_x / 8));
        int8_t tile_num = signed_tiles ? (int8_t)bus_read(tile_addr) : bus_read(tile_addr);

        uint16_t tile_data_addr = signed_tiles
            ? 0x9000 + (tile_num * 16)
            : 0x8000 + (tile_num * 16);

        uint8_t line = pixel_y % 8;
        uint8_t data1 = bus_read(tile_data_addr + line * 2);
        uint8_t data2 = bus_read(tile_data_addr + line * 2 + 1);

        uint8_t bit = 7 - (pixel_x % 8);
        uint8_t color_num = ((data2 >> bit) & 1) << 1 | ((data1 >> bit) & 1);
        uint8_t color = (bg_palette >> (color_num * 2)) & 3;

        uint32_t rgb = color == 0 ? 0x00FFFFFF :
            color == 1 ? 0x00AAAAAA :
            color == 2 ? 0x00555555 : 0x00000000;

        ctx.framebuffer[ctx.line * 160 + x] = rgb | 0xFF000000;
    }
}

gpu_context* gpu_get_context(void) {
    return &ctx;
}

bool gpu_in_vblank(void) {
    return ctx.mode == 1;
}