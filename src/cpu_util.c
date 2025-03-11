/****************************** CameCore *********************************
 *
 * Module: CPU Utilities
 *
 * Key Features:
 * - GetRegister: Returns the value of a specified CPU register (supports both 8-bit and 16-bit registers).
 * - Handles registers such as A, F, B, C, D, E, H, L, AF, BC, DE, HL, PC, and SP.
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
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
#define REVERSE( n )      ( (u16)( ( LOW_BYTE( n ) << 8 ) | HIGH_BYTE( n ) ) ) /**< Swap high/low bytes of a 16-bit */
#define PTR_TO_U16( ptr ) *( (u16 *)&( ptr ) ) /**< Cast address to u16 pointer and dereference */

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
extern CPUContext cpu_ctx;

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
u16  GetRegister( RegType rt );
void SetRegister( RegType rt, u16 val );

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the register data by the given `RegType`
u16
GetRegister( RegType rt )
{
    switch( rt )
        {
            case RT_A:  return cpu_ctx.regs.a;
            case RT_F:  return cpu_ctx.regs.f;
            case RT_B:  return cpu_ctx.regs.b;
            case RT_C:  return cpu_ctx.regs.c;
            case RT_D:  return cpu_ctx.regs.d;
            case RT_E:  return cpu_ctx.regs.e;
            case RT_H:  return cpu_ctx.regs.h;
            case RT_L:  return cpu_ctx.regs.l;

            case RT_AF: return REVERSE( PTR_TO_U16( cpu_ctx.regs.a ) );
            case RT_BC: return REVERSE( PTR_TO_U16( cpu_ctx.regs.b ) );
            case RT_DE: return REVERSE( PTR_TO_U16( cpu_ctx.regs.d ) );
            case RT_HL: return REVERSE( PTR_TO_U16( cpu_ctx.regs.h ) );

            case RT_PC: return cpu_ctx.regs.pc;
            case RT_SP: return cpu_ctx.regs.sp;

            default:    return 0;
        }
}

void
SetRegister( RegType rt, u16 val )
{
    switch( rt )
        {
            case RT_A:    cpu_ctx.regs.a = LOW_BYTE( val ); break;
            case RT_F:    cpu_ctx.regs.f = LOW_BYTE( val ); break;
            case RT_B:    cpu_ctx.regs.b = LOW_BYTE( val ); break;
            case RT_C:    cpu_ctx.regs.c = LOW_BYTE( val ); break;
            case RT_D:    cpu_ctx.regs.d = LOW_BYTE( val ); break;
            case RT_E:    cpu_ctx.regs.e = LOW_BYTE( val ); break;
            case RT_H:    cpu_ctx.regs.h = LOW_BYTE( val ); break;
            case RT_L:    cpu_ctx.regs.l = LOW_BYTE( val ); break;

            case RT_AF:   PTR_TO_U16( cpu_ctx.regs.a ) = REVERSE( val ); break;
            case RT_BC:   PTR_TO_U16( cpu_ctx.regs.b ) = REVERSE( val ); break;
            case RT_DE:   PTR_TO_U16( cpu_ctx.regs.d ) = REVERSE( val ); break;
            case RT_HL:   PTR_TO_U16( cpu_ctx.regs.h ) = REVERSE( val ); break;

            case RT_PC:   cpu_ctx.regs.pc = val; break;
            case RT_SP:   cpu_ctx.regs.sp = val; break;

            case RT_NONE: break;
        }
}
