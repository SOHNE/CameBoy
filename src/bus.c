/**
 *  MEMORY MAP
 *  16-bit address bus
 *
 * +-----------+--------------------------------------+----------------------------------------------+
 * | Range     | Description                          | Notes                                        |
 * +-----------+--------------------------------------+----------------------------------------------+
 * | 0000-3FFF | ROM Bank 0 (16 KiB)                  | Fixed bank from cartridge                    |
 * | 4000-7FFF | ROM Bank 1 (16 KiB)                  | Switchable via mapper (if available)         |
 * | 8000-9FFF | Video RAM (8 KiB total)              | In CGB mode, VRAM bank 0/1 may be switchable |
 * |           |   8000-97FF: CHR RAM                 |                                              |
 * |           |   9800-9BFF: BG Map 1                |                                              |
 * |           |   9C00-9FFF: BG Map 2                |                                              |
 * | A000-BFFF | Cartridge (External) RAM (8 KiB)     | Switchable                                   |
 * | C000-CFFF | Work RAM Bank 0 (WRAM, 4 KiB)        |                                              |
 * | D000-DFFF | Work RAM Bank 1-7 (WRAM, 4 KiB each) | Switchable (Color only / CGB mode)           |
 * | E000-FDFF | Echo RAM                             | Mirror of C000-DDFF; Do not use              |
 * | FE00-FE9F | Object Attribute Memory (OAM)        |                                              |
 * | FEA0-FEFF | Unusable                             | Reserved/Prohibited by Nintendo              |
 * | FF00-FF7F | I/O Registers                        |                                              |
 * | FF80-FFFE | High RAM (HRAM / Zero Page)          |                                              |
 * | FFFF-FFFF | Interrupt Enable Register (IE)       |                                              |
 * +-----------+--------------------------------------+----------------------------------------------+
 * REFERENCE: https://gbdev.io/pandocs/Memory_Map.html
*/

#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------------------------------------------
u8
ReadBus( u16 addr )
{
    if( addr <= ROM_BANKN_END )
        {
            // Cartridge ROM: 0x0000–0x7FFF
            return ReadCartridge( addr );
        }
    else if( addr <= VRAM_END )
        {
            // Video RAM (VRAM): 0x8000–0x9FFF
            NO_IMPL();
        }
    else if( addr <= EXTRAM_END )
        {
            // Cartridge RAM (External RAM): 0xA000–0xBFFF
            NO_IMPL();
        }
    else if( addr <= WRAM_END )
        {
            // Work RAM (WRAM): 0xC000–0xDFFF
            NO_IMPL();
        }
    else if( addr <= ECHO_END )
        {
            // Echo RAM: 0xE000–0xFDFF
            NO_IMPL();
        }
    else if( addr <= OAM_END )
        {
            // Object Attribute Memory (OAM): 0xFE00–0xFE9F
            NO_IMPL();
        }
    else if( addr <= 0xFEFF )
        {
            // Unusable / Reserved memory: 0xFEA0–0xFEFF
            NO_IMPL();
        }
    else if( addr <= IO_END )
        {
            // I/O Registers: 0xFF00–0xFF7F
            NO_IMPL();
        }
    else if( addr <= HRAM_END )
        {
            // High RAM (HRAM): 0xFF80–0xFFFE
            NO_IMPL();
        }
    else if( addr == IE_REGISTER )
        {
            // Interrupt Enable Register: 0xFFFF
            NO_IMPL();
        }

    // High RAM (HRAM): 0xFF80–0xFFFE
    NO_IMPL();
    return 0;
}

void
WriteBus( u16 addr, u8 value )
{
    // Cartridge ROM: 0x0000–0x7FFF
    if( addr <= ROM_BANKN_END )
        {
            WriteCartridge( addr, value );
            return;
        }
    else if( addr <= VRAM_END )
        {
            // Video RAM (VRAM): 0x8000–0x9FFF
            NO_IMPL();
        }
    else if( addr <= EXTRAM_END )
        {
            // Cartridge RAM (External RAM): 0xA000–0xBFFF
            NO_IMPL();
        }
    else if( addr <= WRAM_END )
        {
            // Work RAM (WRAM): 0xC000–0xDFFF
            NO_IMPL();
        }
    else if( addr <= ECHO_END )
        {
            // Echo RAM (mirrors WRAM): 0xE000–0xFDFF
            NO_IMPL();
        }
    else if( addr <= OAM_END )
        {
            // Object Attribute Memory (OAM): 0xFE00–0xFE9F
            NO_IMPL();
        }
    else if( addr <= 0xFEFF )
        {
            // Unusable/Reserved memory: 0xFEA0–0xFEFF
            NO_IMPL();
        }
    else if( addr <= IO_END )
        {
            // I/O Registers: 0xFF00–0xFF7F
            NO_IMPL();
        }
    else if( addr <= HRAM_END )
        {
            // High RAM (HRAM): 0xFF80–0xFFFE
            NO_IMPL();
        }
    else if( addr == IE_REGISTER )
        {
            // Interrupt Enable Register: 0xFFFF
            NO_IMPL();
        }
    else
        {
            // High RAM (HRAM): 0xFF80–0xFFFE
            NO_IMPL();
        }
}
