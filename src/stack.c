/****************************** CameCore *********************************
 *
 * Module: Stack Operations
 *
 * Implements stack manipulation routines for byte and word data types,
 * handling stack pointer management and memory interactions.
 *
 * Key Features:
 * - Byte/word stack push/pop operations
 * - Stack pointer (SP) auto-increment/decrement
 * - Big-endian word handling for stack operations
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

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// ...

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
// Push single byte onto stack
void
PushStack( u8 data )
{
    --GetRegisters()->sp;
    WriteBus( GetRegisters()->sp, data );
}

// Push 16-bit word value (big-endian) by storing high byte first
void
PushStackWord( u16 data )
{
    PushStack( HIGH_BYTE( data ) );
    PushStack( LOW_BYTE( data ) );
}

// Pop single byte from stack
u8
PopStack( void )
{
    return ReadBus( GetRegisters()->sp++ );
}

// Pop 16-bit word value from stack (little-endian order)
u16
PopStackWord( void )
{
    u16 lo;
    u16 hi;

    lo = PopStack();
    hi = PopStack();

    return MAKE_WORD( hi, lo );
}
