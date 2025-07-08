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
#include <stdio.h>

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
#define BOOT_ROM_START_ADDR 0x0100
#define INITIAL_STACK_PTR   (unsigned short)0xFFFE
#define INITIAL_AF          (short)0xB001
#define INITIAL_BC          (short)0x1300
#define INITIAL_DE          (short)0xD800
#define INITIAL_HL          (short)0x4D01

// Flag access
#define GET_FLAG( flag )    BIT_CHECK( cpu_ctx.regs.f, FLAG_##flag##_BIT )
#define FLAG_CHAR( flag )   ( GET_FLAG( flag ) ? #flag[0] : '-' )

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

// Debug related
extern void Disassemble( CPUContext * cpu_ctx, char * str, size_t str_size );

// CPU actions
void CPUInit( void );
bool CPUStep( void );

// Registers
u8   GetIERegister( void );
void SetIERegister( u8 v );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Performs the instruction execution method
static void
Execute( void )
{
    CPUInstructionProc proc = GetInstructionProcessor( cpu_ctx.inst_state.cur_inst->type );

    if( UNLIKELY( NULL == proc ) )
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
    cpu_ctx.regs.pc               = BOOT_ROM_START_ADDR;
    cpu_ctx.regs.sp               = INITIAL_STACK_PTR;
    *( (short *)&cpu_ctx.regs.a ) = INITIAL_AF;
    *( (short *)&cpu_ctx.regs.b ) = INITIAL_BC;
    *( (short *)&cpu_ctx.regs.d ) = INITIAL_DE;
    *( (short *)&cpu_ctx.regs.h ) = INITIAL_HL;
}

// Performs a single CPU step
bool
CPUStep( void )
{
    if( false == cpu_ctx.status.halted )
        {
            const u16 pc = cpu_ctx.regs.pc;

            FetchInstruction();
            AddEmulatorCycles( 1 );
            FetchData();

#if defined( LOG_CPU_INSTR )
            {
                char inst[32];
                Disassemble( &cpu_ctx, inst, sizeof( inst ) );

                LOG( LOG_INFO, "%08llX PC:%04X | %s | A:%02X F:%c%c%c%c | BC:%02X%02X DE:%02X%02X HL:%02X%02X",
                     GetEmulatorContext()->ticks, pc, inst, cpu_ctx.regs.a, FLAG_CHAR( Z ), FLAG_CHAR( N ),
                     FLAG_CHAR( H ), FLAG_CHAR( C ), cpu_ctx.regs.b, cpu_ctx.regs.c, cpu_ctx.regs.d, cpu_ctx.regs.e,
                     cpu_ctx.regs.h, cpu_ctx.regs.l );
            }
#endif

            if( UNLIKELY( NULL == cpu_ctx.inst_state.cur_inst ) )
                {
                    LOG( LOG_FATAL, "Unknown Instruction! %02X\n", cpu_ctx.inst_state.cur_opcode );
                    return false;
                }

            Execute();
        }
    return true;
}

// Get the Interrupt Enable(IE) register
u8
GetIERegister( void )
{
    return cpu_ctx.interupt_state.ie_reg;
}

// Set the Interrupt Enable(IE) register
void
SetIERegister( u8 v )
{
    cpu_ctx.interupt_state.ie_reg = v;
}

// Retrieve the CPU registers pointer
CPURegisters *
GetRegisters( void )
{
    return &cpu_ctx.regs;
}
