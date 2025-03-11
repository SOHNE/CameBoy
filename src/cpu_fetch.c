/****************************** CameCore *********************************
 *
 * Module: CPU Fetch
 *
 * It provides functionality to fetch the next instruction
 * opcode and associated data, handling addressing modes.
 *
 * Key Functions:
 * - FetchInstruction: Retrieves the next opcode and corresponding instruction.
 * - FetchData: Retrieves immediate data or register content based on addressing mode.
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
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
extern CPUContext cpu_ctx;

//----------------------------------------------------------------------------------------------------------------------
// Address Mode Handler Function Declarations
//----------------------------------------------------------------------------------------------------------------------
typedef void ( *AddressModeHandler )( void );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
// CPU Fetch
void FetchInstruction( void );
void FetchData( void );

extern Instruction * GetInstructionByOpCode( u8 opcode ); // Get the given opcode `Instruction`
extern u16           GetRegister( RegType rt );           // Get the given register data
extern void          SetRegister( RegType rt, u16 val );  // Get the given register data

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Fetch 16-bit little-endian from pc (lo, hi)
static inline u16
FETCH_LO_HI( u16 pc )
{
    u8 lo;
    u8 hi;

    /* Get low byte */
    lo = ReadBus( pc );
    AddEmulatorCycles( 1 );

    /* Get high byte */
    hi = ReadBus( pc + 1 );
    AddEmulatorCycles( 1 );

    return (u16)( lo | ( hi << 8 ) );
}

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Implied addressing: no operand
static void
AM_Handler_IMP( void )
{
    return;
}

// Register addressing: data in register
static void
AM_Handler_R( void )
{
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
}

// Register to register addressing
static void
AM_Handler_R_R( void )
{
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
}

// Register + 8-bit immediate
static void
AM_Handler_R_D8( void )
{
    cpu_ctx.inst_state.fetched_data = ReadBus( cpu_ctx.regs.pc );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
}

// Register + 16-bit immediate
static void
AM_Handler_R_D16( void )
{
    cpu_ctx.inst_state.fetched_data  = FETCH_LO_HI( cpu_ctx.regs.pc );
    cpu_ctx.regs.pc                 += 2;
}

// 16-bit immediate address
static void
AM_Handler_D16( void )
{
    cpu_ctx.inst_state.fetched_data  = FETCH_LO_HI( cpu_ctx.regs.pc );
    cpu_ctx.regs.pc                 += 2;
}

// Memory address in register + register data
static void
AM_Handler_MR_R( void )
{
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
    cpu_ctx.inst_state.mem_dest     = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
    cpu_ctx.inst_state.dest_is_mem  = true;

    if( RT_C == cpu_ctx.inst_state.cur_inst->primary_reg )
        {
            cpu_ctx.inst_state.mem_dest |= 0xFF00;
        }
}

// Register + memory address in register
static void
AM_Handler_R_MR( void )
{
    u16 addr = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );

    if( RT_C == cpu_ctx.inst_state.cur_inst->secondary_reg )
        {
            addr |= 0xFF00;
        }

    cpu_ctx.inst_state.fetched_data = ReadBus( addr );
    AddEmulatorCycles( 1 );
}

// Register + (HL), increment HL
static void
AM_Handler_R_HLI( void )
{
    cpu_ctx.inst_state.fetched_data = ReadBus( GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg ) );
    AddEmulatorCycles( 1 );
    SetRegister( RT_HL, GetRegister( RT_HL ) + 1 );
}

// Register + (HL), decrement HL
static void
AM_Handler_R_HLD( void )
{
    cpu_ctx.inst_state.fetched_data = ReadBus( GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg ) );
    AddEmulatorCycles( 1 );
    SetRegister( RT_HL, GetRegister( RT_HL ) - 1 );
}

// (HL) + register, increment HL
static void
AM_Handler_HLI_R( void )
{
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
    cpu_ctx.inst_state.mem_dest     = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
    cpu_ctx.inst_state.dest_is_mem  = true;
    SetRegister( RT_HL, GetRegister( RT_HL ) + 1 );
}

// (HL) + register, decrement HL
static void
AM_Handler_HLD_R( void )
{
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
    cpu_ctx.inst_state.mem_dest     = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
    cpu_ctx.inst_state.dest_is_mem  = true;
    SetRegister( RT_HL, GetRegister( RT_HL ) - 1 );
}

// Register + 8-bit address offset
static void
AM_Handler_R_A8( void )
{
    u16 addr                        = ReadBus( cpu_ctx.regs.pc ) | 0xFF00;
    cpu_ctx.inst_state.fetched_data = ReadBus( addr );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
}

// 8-bit address offset + register
static void
AM_Handler_A8_R( void )
{
    cpu_ctx.inst_state.mem_dest     = ReadBus( cpu_ctx.regs.pc ) | 0xFF00;
    cpu_ctx.inst_state.dest_is_mem  = true;
    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
}

// HL + SP + register
static void
AM_Handler_HL_SPR( void )
{
    cpu_ctx.inst_state.fetched_data = ReadBus( cpu_ctx.regs.pc );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
}

static void
AM_Handler_D8( void )
{
    // 8-bit immediate data
    cpu_ctx.inst_state.fetched_data = ReadBus( cpu_ctx.regs.pc );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
}

// 16-bit address + register
static void
AM_Handler_A16_R( void )
{
    u16 addr                         = FETCH_LO_HI( cpu_ctx.regs.pc );
    cpu_ctx.regs.pc                 += 2;

    cpu_ctx.inst_state.mem_dest      = addr;
    cpu_ctx.inst_state.dest_is_mem   = true;

    cpu_ctx.inst_state.fetched_data  = GetRegister( cpu_ctx.inst_state.cur_inst->secondary_reg );
}

// Same implementation as A16_R
static void
AM_Handler_D16_R( void )
{
    AM_Handler_A16_R();
}

// Memory address in register + 8-bit immediate
static void
AM_Handler_MR_D8( void )
{
    cpu_ctx.inst_state.fetched_data = ReadBus( cpu_ctx.regs.pc );
    AddEmulatorCycles( 1 );
    ++cpu_ctx.regs.pc;
    cpu_ctx.inst_state.mem_dest    = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
    cpu_ctx.inst_state.dest_is_mem = true;
}

// Memory address in register
static void
AM_Handler_MR( void )
{
    cpu_ctx.inst_state.mem_dest     = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
    cpu_ctx.inst_state.dest_is_mem  = true;
    cpu_ctx.inst_state.fetched_data = ReadBus( GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg ) );
    AddEmulatorCycles( 1 );
}

// Register + 16-bit address
static void
AM_Handler_R_A16( void )
{
    u16 addr                         = FETCH_LO_HI( cpu_ctx.regs.pc );
    cpu_ctx.regs.pc                 += 2;

    cpu_ctx.inst_state.fetched_data  = ReadBus( addr );
    AddEmulatorCycles( 1 );
}

// Unknown addressing mode handler
static void
AM_Handler_UNKNOWN( void )
{
    LOG( LOG_FATAL, "Unknown Addressing Mode! %d (%02X)\n", cpu_ctx.inst_state.cur_inst->addr_mode,
         cpu_ctx.inst_state.cur_opcode );
}

//----------------------------------------------------------------------------------------------------------------------
// Address Mode Handler Table
//----------------------------------------------------------------------------------------------------------------------
static AddressModeHandler ADDRESS_MODE_HANDLERS[] = {

#define AM_HANDLER( mode ) [AM_##mode] = AM_Handler_##mode
    AM_HANDLER( IMP ),   AM_HANDLER( R ),     AM_HANDLER( R_R ),   AM_HANDLER( R_D8 ),  AM_HANDLER( R_D16 ),
    AM_HANDLER( D16 ),   AM_HANDLER( MR_R ),  AM_HANDLER( R_MR ),  AM_HANDLER( R_HLI ), AM_HANDLER( R_HLD ),
    AM_HANDLER( HLI_R ), AM_HANDLER( HLD_R ), AM_HANDLER( R_A8 ),  AM_HANDLER( A8_R ),  AM_HANDLER( HL_SPR ),
    AM_HANDLER( D8 ),    AM_HANDLER( A16_R ), AM_HANDLER( D16_R ), AM_HANDLER( MR_D8 ), AM_HANDLER( MR ),
    AM_HANDLER( R_A16 )
#undef AM_HANDLER

};

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the next instruction opcode and `Instruction`
void
FetchInstruction( void )
{
    cpu_ctx.inst_state.cur_opcode = ReadBus( cpu_ctx.regs.pc++ );
    cpu_ctx.inst_state.cur_inst   = GetInstructionByOpCode( cpu_ctx.inst_state.cur_opcode );
}

// Retrieve the current instruction data
// TODO: Review the AM executions
void
FetchData( void )
{
    cpu_ctx.inst_state.mem_dest    = 0;
    cpu_ctx.inst_state.dest_is_mem = false;

    if( NULL == cpu_ctx.inst_state.cur_inst ) return;

    // Get addressing mode from current instruction
    AddrMode mode = cpu_ctx.inst_state.cur_inst->addr_mode;

    // Check if mode is valid
    if( false == INDEX_VALID( mode, ADDRESS_MODE_HANDLERS ) )
        {
            AM_Handler_UNKNOWN();
            return;
        }

    // Call the appropriate handler function from the lookup table
    ADDRESS_MODE_HANDLERS[mode]();
}
