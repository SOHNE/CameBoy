/****************************** CameCore *********************************
 *
 * Module: Core
 *
 * It provides emulation initialization, CPU stepping, cycle management,
 * and runtime state control.
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *      wrote the original software. If you use this software in a product, an acknowledgment
 *      in the product documentation would be appreciated but is not required.
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
