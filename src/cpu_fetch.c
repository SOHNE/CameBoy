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
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
extern Instruction * GetInstructionByOpCode( u8 opcode ); // Get the given opcode `Instruction`
extern u16           GetRegister( RegType rt );           // Get the given register data

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Fetch 16-bit little-endian from pc (lo, hi)
static inline u16
FETCH_LO_HI( u16 pc )
{
    u8 lo = ReadBus( pc );
    AddEmulatorCycles( 1 );

    u8 hi = ReadBus( pc + 1 );
    AddEmulatorCycles( 1 );

    return lo | ( hi << 8 );
}

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
void
FetchData( void )
{
    cpu_ctx.inst_state.mem_dest    = 0;
    cpu_ctx.inst_state.dest_is_mem = false;

    if( NULL == cpu_ctx.inst_state.cur_inst ) return;

    switch( cpu_ctx.inst_state.cur_inst->addr_mode )
        {
            case AM_IMP: /** Implied addressing: no operand */ return;

            case AM_R:
                /** Register addressing: data in register */
                {
                    cpu_ctx.inst_state.fetched_data = GetRegister( cpu_ctx.inst_state.cur_inst->primary_reg );
                    return;
                }

            case AM_R_D8:
                /** Register + 8-bit immediate */
                {
                    cpu_ctx.inst_state.fetched_data = ReadBus( cpu_ctx.regs.pc );
                    AddEmulatorCycles( 1 );
                    ++cpu_ctx.regs.pc;
                    return;
                }

            case AM_D16:
                /** 16-bit immediate address */
                {
                    cpu_ctx.inst_state.fetched_data  = FETCH_LO_HI( cpu_ctx.regs.pc );
                    cpu_ctx.regs.pc                 += 2;

                    return;
                }

            default:
                /** Unknown addressing mode handler */
                {
                    LOG( LOG_FATAL, "Unknown Addressing Mode! %d (%02X)\n", cpu_ctx.inst_state.cur_inst->addr_mode,
                         cpu_ctx.inst_state.cur_opcode );
                    return;
                }
        }
}
