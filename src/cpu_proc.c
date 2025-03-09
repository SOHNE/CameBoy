/****************************** CameCore *********************************
 *
 * Module: CPU Instruction Processors
 *
 * Implements the CPU instruction processors.
 * It provides:
 *   - Flag access macros for efficient bit manipulation.
 *   - A function to check instruction conditions based on CPU flags.
 *   - Implementations for several CPU instructions (e.g., NOP, DI, LD, XOR, JP).
 *   - A lookup table mapping instruction types to their corresponding processor functions.
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
// Flag access
#define GET_FLAG( flag )        BIT_CHECK( cpu_ctx->regs.f, FLAG_##flag##_BIT )
#define SET_FLAG( flag, value ) BIT_ASSIGN( cpu_ctx->regs.f, FLAG_##flag##_BIT, value )

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
CPUInstructionProc GetInstructionProcessor( InsType type );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Return the check condition based on the given current instruction condition type
static bool
CheckCond( CPUContext * cpu_ctx )
{
    const bool z = GET_FLAG( Z );
    const bool c = GET_FLAG( C );

    switch( cpu_ctx->inst_state.cur_inst->condition_type )
        {
            case CT_NONE: return true;
            case CT_C:    return c;
            case CT_NC:   return !c;
            case CT_Z:    return z;
            case CT_NZ:   return !z;
            default:      return false;
        }
    return false;
}

// Instructions Definition
//------------------------------------------------------------------
/**
 * Invalid Instruction Handler
 *
 * Logs a fatal error when an invalid/unimplemented opcode is encountered.
 * Does not modify any CPU state or flags.
 *
 * Z N H C
 * - - - -
 */
static void
ProcNONE( CPUContext * cpu_ctx )
{
    UNUSED( cpu_ctx );
    LOG( LOG_FATAL, "INVALID INSTRUCTION" );
}

/*
 * Mnemonic    : NOP
 * Instruction : Only advances the program counter by 1.
 * Function    :
 *
 * Z N H C
 * - - - -
 */
static void
ProcNOP( CPUContext * cpu_ctx )
{
    UNUSED( cpu_ctx );
}

/*
 * Mnemonic    : DI
 * Instruction : Reset the interrupt master enable (IME) flag
 * Function    : IME = 0
 *
 * Z N H C
 * - - - -
 */
static void
ProcDI( CPUContext * cpu_ctx )
{
    cpu_ctx->interupt_state.ime = false;
}

/*
 * Mnemonic    : LD
 * Instruction : Load
 * Function    : R = O
 *
 * Z N H C
 * - - - -
 */
static void
ProcLD( CPUContext * cpu_ctx )
{
    UNUSED( cpu_ctx );
}

/*
 * Mnemonic    : XOR
 * Instruction : Logical Exclusive OR
 * Function    : A = A ^ OP
 *
 * Z N H C
 * + 0 0 0
 */
static void
ProcXOR( CPUContext * cpu_ctx )
{
    cpu_ctx->regs.a ^= LOW_BYTE( cpu_ctx->inst_state.fetched_data );

    SET_FLAG( Z, cpu_ctx->regs.a == 0 );
    SET_FLAG( N, 0 );
    SET_FLAG( H, 0 );
    SET_FLAG( C, 0 );
}

/*
 * Mnemonic    : JP
 * Instruction : Jump To Location
 * Function    : pc = address
 *
 * Z N H C
 * - - - -
 */
static void
ProcJP( CPUContext * cpu_ctx )
{
    if( false == CheckCond( cpu_ctx ) ) return;

    cpu_ctx->regs.pc = cpu_ctx->inst_state.fetched_data;
    AddEmulatorCycles( 1 );
}

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// Define the processors array and populate it with instruction processor functions.
#define PROC( mnemonic ) [INS_##mnemonic] = Proc##mnemonic

static CPUInstructionProc PROCESSORS[] = {
    PROC( NONE ), PROC( NOP ), PROC( LD ), PROC( JP ), PROC( DI ), PROC( XOR ),
};

#undef PROC

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the given instruction execution method
CPUInstructionProc
GetInstructionProcessor( InsType type )
{
    return PROCESSORS[type];
}
