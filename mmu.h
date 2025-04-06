#pragma once
#include "common.h"

#define MEMORY_SIZE 0x10000

extern uint8_t memory[MEMORY_SIZE];

void mmu_init();
uint8_t mmu_read(uint16_t addr);
void mmu_write(uint16_t addr, uint8_t val);