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

extern u16  GetRegister( RegType rt );
extern void SetRegister( RegType rt, u16 val );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Return the check condition based on the given current instruction condition type
static bool
CheckCondition( CPUContext * cpu_ctx )
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

// Instructions Implementation
//------------------------------------------------------------------

/**
 * Invalid Instruction Handler
 *
 * Logs a fatal error when an invalid/unimplemented opcode is encountered.
 * Exits the program with error code -7.
 *
 * Z N H C
 * - - - -
 */
static void
ProcNONE( CPUContext * cpu_ctx )
{
    UNUSED( cpu_ctx );
    LOG( LOG_FATAL, "INVALID INSTRUCTION!\n" );
}

/**
 * Mnemonic    : NOP
 * Instruction : No Operation
 * Function    : Does nothing
 *
 * Z N H C
 * - - - -
 */
static void
ProcNOP( CPUContext * cpu_ctx )
{
    UNUSED( cpu_ctx );
}

/**
 * Mnemonic    : DI
 * Instruction : Disable Interrupts
 * Function    : Disables interrupt master enable flag
 *
 * Z N H C
 * - - - -
 */
static void
ProcDI( CPUContext * cpu_ctx )
{
    cpu_ctx->interupt_state.ime = false;
}

/**
 * Mnemonic    : LD
 * Instruction : Load
 * Function    : Loads data into register or memory
 *
 * Z N H C
 * - - - -
 * (flags affected for specific LD operations)
 */
// TODO: Fix the conversion loses at SetRegister
static void
ProcLD( CPUContext * cpu_ctx )
{
    // If destination is memory, perform a memory write
    if( true == cpu_ctx->inst_state.dest_is_mem )
        {
            // LD (destination), source
            if( RT_AF <= cpu_ctx->inst_state.cur_inst->secondary_reg )
                {
                    // 16-bit register: add a cycle and write 16 bits
                    AddEmulatorCycles( 1 );
                    WriteBus16( cpu_ctx->inst_state.mem_dest, cpu_ctx->inst_state.fetched_data );
                }
            else
                {
                    // 8-bit register: write only the lower byte
                    WriteBus( cpu_ctx->inst_state.mem_dest, LOW_BYTE( cpu_ctx->inst_state.fetched_data ) );
                }
            return;
        }

    // Handle special case: HL = SP + r8 addressing mode
    if( AM_HL_SPR == cpu_ctx->inst_state.cur_inst->addr_mode )
        {
            // Compute half-carry flag: if lower nibble sum is at least 0x10
            u8 hflag = ( 0x10 <= ( ( ( GetRegister( cpu_ctx->inst_state.cur_inst->secondary_reg ) & 0xF ) +
                                     ( cpu_ctx->inst_state.fetched_data & 0xF ) ) ) );

            // Compute carry flag: if full byte sum is at least 0x100
            u8 cflag = ( 0x100 <= ( ( ( GetRegister( cpu_ctx->inst_state.cur_inst->secondary_reg ) & 0xFF ) +
                                      ( cpu_ctx->inst_state.fetched_data & 0xFF ) ) ) );

            // Set flags
            SET_FLAG( Z, 0 );
            SET_FLAG( N, 0 );
            SET_FLAG( H, hflag );
            SET_FLAG( C, cflag );

            SetRegister( cpu_ctx->inst_state.cur_inst->primary_reg,
                         GetRegister( cpu_ctx->inst_state.cur_inst->secondary_reg ) +
                             (char)cpu_ctx->inst_state.fetched_data );
            return;
        }

    // Standard register load: simply move the fetched data to the target register
    SetRegister( cpu_ctx->inst_state.cur_inst->primary_reg, cpu_ctx->inst_state.fetched_data );
}

/**
 * Mnemonic    : LDH
 * Instruction : Load High
 * Function    : Special load instructions for accessing the high memory area (0xFF00-0xFFFF)
 *
 * Z N H C
 * - - - -
 */
static void
ProcLDH( CPUContext * ctx )
{
    if( RT_A == ctx->inst_state.cur_inst->primary_reg )
        {
            // LDH A, (n) instruction - Load from high memory into A
            // Opcode: 0xF0
            // Loads the contents of memory at address (0xFF00 + n) into register A
            // Used to access hardware registers in Game Boy's memory map
            SetRegister( ctx->inst_state.cur_inst->primary_reg, ReadBus( 0xFF00 | ctx->inst_state.fetched_data ) );
        }
    else
        {
            // LDH (n), A instruction - Store A into high memory
            // Opcode: 0xE0
            // Stores the contents of register A into memory at address (0xFF00 + n)
            // Common usage is for hardware I/O registers like joypad, serial, timer controls
            WriteBus( 0xFF00 | ctx->inst_state.fetched_data, ctx->regs.a );
        }

    AddEmulatorCycles( 1 );
}

/**
 * Mnemonic    : XOR
 * Instruction : Logical XOR
 * Function    : A = A ^ operand
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

/**
 * Mnemonic    : JP
 * Instruction : Jump
 * Function    : PC = address if condition is met
 *
 * Z N H C
 * - - - -
 */
static void
ProcJP( CPUContext * cpu_ctx )
{
    if( CheckCondition( cpu_ctx ) )
        {
            cpu_ctx->regs.pc = cpu_ctx->inst_state.fetched_data;
            AddEmulatorCycles( 1 );
        }
}

//----------------------------------------------------------------------------------------------------------------------
// Processor Table
//----------------------------------------------------------------------------------------------------------------------
static CPUInstructionProc PROCESSORS[] = {

#define PROC( mnemonic ) [INS_##mnemonic] = Proc##mnemonic
    PROC( NONE ), PROC( NOP ), PROC( LD ), PROC( JP ), PROC( DI ), PROC( LDH ), PROC( XOR ),
#undef PROC

};

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------

CPUInstructionProc
GetInstructionProcessor( InsType type )
{
    return PROCESSORS[type];
}
