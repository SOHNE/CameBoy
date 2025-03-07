#include "camecore/camecore.h"
#include "camecore/utils.h"

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

            {
                const CPURegisters regs = cpu_ctx.regs;
                LOG( LOG_INFO, "%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X", regs.pc,
                     GetInstructionName( cpu_ctx.inst_state.cur_inst->type ), cpu_ctx.inst_state.cur_opcode,
                     ReadBus( regs.pc + 1 ), ReadBus( regs.pc + 2 ), regs.a, regs.b, regs.c );
            }

            if( NULL == cpu_ctx.inst_state.cur_inst )
                {
                    LOG( LOG_FATAL, "Unknown Instruction! %02X\n", cpu_ctx.inst_state.cur_opcode );
                }

            Execute();
        }

    return true;
}
