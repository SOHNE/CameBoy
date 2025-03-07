#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
// Flag access functions that use your existing bit operations
#define GET_FLAG( flag )        BIT_CHECK( cpu_ctx->regs.f, FLAG_##flag##_BIT )
#define SET_FLAG( flag, value ) BIT_ASSIGN( cpu_ctx->regs.f, FLAG_##flag##_BIT, value )

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
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
//Retrieve the given instruction execution method
CPUInstructionProc
GetInstructionProcessor( InsType type )
{
    switch( type )
        {
            case INS_NONE: return ProcNONE;
            case INS_NOP:  return ProcNOP;
            case INS_LD:   return ProcLD;
            case INS_JP:   return ProcJP;
            case INS_DI:   return ProcDI;
            case INS_XOR:  return ProcXOR;

            default:       return ProcNONE;
        }
}
