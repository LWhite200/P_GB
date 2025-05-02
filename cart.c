#include <stdio.h>
#include <stdlib.h>
#include "cart.h"

unsigned char* load_rom(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    unsigned char* buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    fclose(file);

    *out_size = size;
    return buffer;
}
