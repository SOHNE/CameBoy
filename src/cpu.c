/****************************** CameCore *********************************
 *
 * Module: CPU
 *
 * It defines the CPU context, handles instruction
 * fetching, decoding, and execution, and provides detailed logging for
 * debugging and tracing CPU activity.
 *
 * Key Features:
 * - CPU context and register initialization
 * - Instruction fetching and data retrieval from memory
 * - Dynamic instruction execution via function pointers
 * - Logging of instruction execution details
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
#define GET_FLAG( flag )  BIT_CHECK( cpu_ctx.regs.f, FLAG_##flag##_BIT )
#define FLAG_CHAR( flag ) ( GET_FLAG( flag ) ? #flag[0] : '-' )

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
CPUContext cpu_ctx = { 0 };

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
// Instruction related
// NOTE: Defined in `cpu_instr.c`
extern CPUInstructionProc GetInstructionProcessor( InsType type ); // Get Instruction execution method
extern char *             GetInstructionName( InsType t );         // Get the given instruction name

// Fetch related
// NOTE: Defined in `cpu_fetch.c`
extern void FetchInstruction( void ); // Fetch next instruction
extern void FetchData( void );        // Fetch current instruction data

// CPU actions
void CPUInit( void );
bool CPUStep( void );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Performs the instruction execution method
static void
Execute( void )
{
    CPUInstructionProc proc = GetInstructionProcessor( cpu_ctx.inst_state.cur_inst->type );

    if( !proc )
        {
            NO_IMPL();
        }

    proc( &cpu_ctx );
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Initialize the CPU
void
CPUInit( void )
{
    cpu_ctx.regs.pc = 0x100;
    cpu_ctx.regs.a  = 0x01;
}

// Performs a single CPU step
bool
CPUStep( void )
{
    if( !cpu_ctx.status.halted )
        {
            FetchInstruction();
            FetchData();

            /** DEBUG */
            {
                const CPURegisters regs = cpu_ctx.regs;

                LOG( LOG_INFO,
                     "%08llX PC:%04X | %-12s %02X %02X %02X | A:%02X F:%c%c%c%c | BC:%02X%02X DE:%02X%02X "
                     "HL:%02X%02X",
                     GetEmulatorContext()->ticks, regs.pc, GetInstructionName( cpu_ctx.inst_state.cur_inst->type ),
                     cpu_ctx.inst_state.cur_opcode, ReadBus( regs.pc + 1 ), ReadBus( regs.pc + 2 ), regs.a,
                     FLAG_CHAR( Z ), FLAG_CHAR( N ), FLAG_CHAR( H ), FLAG_CHAR( C ), regs.b, regs.c, regs.d, regs.e,
                     regs.h, regs.l );
            }

            if( NULL == cpu_ctx.inst_state.cur_inst )
                {
                    LOG( LOG_FATAL, "Unknown Instruction! %02X\n", cpu_ctx.inst_state.cur_opcode );
                }

            Execute();
        }

    return true;
}
