/****************************** CameCore *********************************
 *
 * Module: RAM
 *
 * Handles the Work RAM (WRAM) and High RAM (HRAM) operations including
 * read/write operations with address validation and memory management.
 *
 * Key Features:
 * - WRAM and HRAM memory management
 * - Address translation and bounds checking
 * - Read/Write operations with error logging
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the use
 * of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including
 * commercial applications, and to alter it and redistribute it freely, subject to the
 * following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that
 *      you wrote the original software. If you use this software in a product, an
 *      acknowledgment in the product documentation would be appreciated but is not required.
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
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Structs Definition
//----------------------------------------------------------------------------------------------------------------------
typedef struct RAMContext
{
    u8 wram[WRAM_SIZE];
    u8 hram[HRAM_SIZE];
} RAMContext;

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
static RAMContext ram_ctx = { 0 };

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Perform read operation to the Work RAM
u8
ReadWRAM( u16 addr )
{
    addr -= WRAM_START;
    ASSERT( WRAM_SIZE > addr, "INVALID WRAM ADDRESS %08X", addr + WRAM_START );

    return ram_ctx.wram[addr];
}

// Perform write operation to the Work RAM
void
WriteWRAM( u16 addr, u8 value )
{
    addr               -= WRAM_START;
    ram_ctx.wram[addr]  = value;
}

// Perform read operation to the High RAM
u8
ReadHRAM( u16 addr )
{
    addr -= HRAM_START;
    ASSERT( HRAM_SIZE > addr, "INVALID HRAM ADDRESS %08X", addr + WRAM_START );

    return ram_ctx.hram[addr];
}

// Perform write operation to the High RAM
void
WriteHRAM( u16 addr, u8 value )
{
    addr               -= HRAM_START;
    ram_ctx.hram[addr]  = value;
}
