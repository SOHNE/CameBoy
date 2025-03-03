#include "camecore/camecore.h"
#include "camecore/utils.h"

static CPUContext ctx = { 0 };

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------
extern CPUInstructionProc GetInstructionProcessor( InsType type );

void
CPUInit( void )
{
    ctx.regs.pc = 0x100;
    ctx.regs.a  = 0x01;

    LOG( LOG_INFO, "CPU Initialized" );
}
