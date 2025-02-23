#include "gbCEmu/gbCEmu.h"
#include <stdio.h>
#include <stdlib.h>

void gb_init(GameBoy* gb) {
    gb->pc = 0x0100;  // Typical starting address
    // Initialize other hardware components...
}

void gb_load_rom(GameBoy* gb, const char* filename) {
    FILE* rom_file = fopen(filename, "rb");
    if (rom_file) {
        fread(gb->rom, 1, sizeof(gb->rom), rom_file);
        fclose(rom_file);
    }
}

void gb_step(GameBoy* gb) {
    // Fetch and execute instruction
    uint8_t opcode = gb->memory[gb->pc++];
    (void) opcode;
    // Add instruction decoding here...
}
