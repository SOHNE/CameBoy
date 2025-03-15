/****************************** CameCore *********************************
 *
 * Module: Bus
 *
 * It manages a 16-bit address bus divided into designated regions including:
 * - Cartridge ROM and RAM
 * - Video RAM (VRAM)
 * - Work RAM (WRAM) and its Echo
 * - I/O Registers and High RAM (HRAM)
 * - Interrupt Enable Register (IE)
 *
 * Memory Map Layout:
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
 * Reference: https://gbdev.io/pandocs/Memory_Map.html
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *      wrote the original software. If you use this software in a product, an acknowledgment
 *      in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *      as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 *
 *************************************************************************/

#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
extern u8   GetIERegister( void );
extern void SetIERegister( u8 v );

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
            return ReadCartridge( addr );
        }
    else if( addr <= WRAM_END )
        {
            // Work RAM (WRAM): 0xC000–0xDFFF
            return ReadWRAM( addr );
        }
    else if( addr <= ECHO_END )
        {
            // Echo RAM: 0xE000–0xFDFF
            return 0;
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
            return ReadHRAM( addr );
        }
    else if( addr == IE_REGISTER )
        {
            // Interrupt Enable Register: 0xFFFF
            return GetIERegister();
        }

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
            WriteCartridge( addr, value );
        }
    else if( addr <= WRAM_END )
        {
            // Work RAM (WRAM): 0xC000–0xDFFF
            WriteWRAM( addr, value );
        }
    else if( addr <= ECHO_END )
        {
            // Echo RAM: 0xE000–0xFDFF
            return;
        }
    else if( addr <= OAM_END )
        {
            // Object Attribute Memory (OAM): 0xFE00–0xFE9F
            NO_IMPL();
        }
    else if( addr <= 0xFEFF )
        {
            // Unusable / Reserved memory: 0xFEA0–0xFEFF
            return;
        }
    else if( addr <= IO_END )
        {
            // I/O Registers: 0xFF00–0xFF7F
            LOG( LOG_ERROR, "IO not mapped [%04X]", addr );
        }
    else if( addr <= HRAM_END )
        {
            // High RAM (HRAM): 0xFF80–0xFFFE
            WriteHRAM( addr, value );
            return;
        }
    else if( addr == IE_REGISTER )
        {
            // Interrupt Enable Register: 0xFFFF
            SetIERegister( value );
        }
}

u16
ReadBus16( u16 address )
{
    u16 lo = ReadBus( address );
    u16 hi = ReadBus( address + 1 );

    return (u16)( lo | ( hi << 8 ) );
}

void
WriteBus16( u16 address, u16 value )
{
    WriteBus( address + 1, HIGH_BYTE( value ) );
    WriteBus( address, LOW_BYTE( value ) );
}
