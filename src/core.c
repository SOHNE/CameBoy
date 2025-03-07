#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
EmuContext ctx = { 0 };

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
extern void CPUInit( void );
extern bool CPUStep( void );

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Initialize the emulation
void
InitEmulator( void )
{
    LOG( LOG_INFO, "Initializing CameCore %s", CAMECORE_VERSION );

    CPUInit();

    ctx.running = true;
    ctx.paused  = false;
    ctx.ticks   = 0;
}

// Step once the emulation execution
bool
StepEmulator( void )
{
    if( false == CPUStep() )
        {
            LOG( LOG_INFO, "CPU Stopped" );
            return false;
        }

    ++ctx.ticks;
    return true;
}

// TODO: Process N CPU cycles (4 ticks/cycle: timers+PPU+APU per tick, DMA post-cycle)
void
AddEmulatorCycles( u32 cpu_cycles )
{
    UNUSED( cpu_cycles );
}

// Get the current running state of the emulation
bool
IsEmulatorRunning( void )
{
    return ctx.running;
}
