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
    u8 lo = BusRead( pc );
    Cycles( 1 );

    u8 hi = BusRead( pc + 1 );
    Cycles( 1 );

    return lo | ( hi << 8 );
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the next instruction opcode and `Instruction`
void
FetchInstruction( void )
{
    cpu_ctx.inst_state.cur_opcode = BusRead( cpu_ctx.regs.pc++ );
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
                    cpu_ctx.inst_state.fetched_data = BusRead( cpu_ctx.regs.pc );
                    Cycles( 1 );
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
