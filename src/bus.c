/** REFERENCE: https://gbdev.io/pandocs/Memory_Map.html */

#include "camecore/camecore.h"
#include "camecore/utils.h"

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
*/

u8
BusRead( u16 addr )
{
    // ROM data
    if( addr < 0x8000 )
        {
            return CartRead( addr );
        }

    NO_IMPL();
    return 0;
}

void
BusWrite( u16 addr, u8 value )
{
    // ROM data
    if( addr < 0x8000 )
        {
            CartWrite( addr, value );
            return;
        }

    NO_IMPL();
}
