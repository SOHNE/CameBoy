#ifndef GBCE_H
#define GBCE_H
#include <stdint.h>

// Basic structure
typedef struct {
  uint8_t memory[0x10000]; // 64KB memory
  uint8_t rom[0x8000];     // 32KB cartridge ROM
  uint16_t pc;             // Program counter
} GameBoy;

// Core functions
void gb_init(GameBoy *gb);
void gb_load_rom(GameBoy *gb, const char *filename);
void gb_step(GameBoy *gb);

#endif // !GBCE_H
