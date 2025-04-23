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
static EmuContext ctx ALIGNED( 16 ) = { 0 }; // Aligned EmuContext
static THREAD_HANDLE  cpu_thread    = { 0 };
static MUTEX_HANDLE   ctx_mutex     = { 0 }; // Mutex to protect access to ctx

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
extern void CPUInit( void );
extern bool CPUStep( void );

static THREAD_RETURN RunCPU( THREAD_PARAM param );

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------

// CPU thread function
static THREAD_RETURN
RunCPU( THREAD_PARAM param )
{
    UNUSED( param );

    // Initialize CPU
    CPUInit();

    // Setup context
    MUTEX_LOCK( ctx_mutex );
    ctx.running = true;
    ctx.paused  = false;
    ctx.ticks   = 0;
    MUTEX_UNLOCK( ctx_mutex );

    bool running = true;
    bool paused  = false;

    while( running )
        {
            // Check running and paused state atomically
            MUTEX_LOCK( ctx_mutex );
            running = ctx.running;
            paused  = ctx.paused;
            MUTEX_UNLOCK( ctx_mutex );

            if( paused )
                {
                    THREAD_SLEEP( 10 );
                    continue;
                }

            if( UNLIKELY( false == CPUStep() ) )
                {
                    LOG( LOG_INFO, "CPU Stopped" );
                    MUTEX_LOCK( ctx_mutex );
                    ctx.running = false;
                    running     = false;
                    MUTEX_UNLOCK( ctx_mutex );
                    break;
                }
        }

#if defined( _WIN32 ) || defined( _WIN64 )
    return 0;
#else
    return NULL;
#endif
}

// Initialize the emulation
void
InitEmulator( void )
{
    LOG( LOG_INFO, "Initializing CameCore %s", CAMECORE_VERSION );

    // Initialize mutex
    MUTEX_INIT( ctx_mutex );

    // Start CPU thread
    THREAD_CREATE( cpu_thread, RunCPU, NULL );
}

// Step once the emulation execution (only used when paused)
bool
StepEmulator( void )
{
    bool result      = false;
    bool should_step = false;

    MUTEX_LOCK( ctx_mutex );
    if( ctx.paused && ctx.running )
        {
            should_step = true;
        }
    MUTEX_UNLOCK( ctx_mutex );

    if( LIKELY( should_step ) )
        {
            result = CPUStep();
            if( UNLIKELY( !result ) )
                {
                    LOG( LOG_INFO, "CPU Stopped" );
                    MUTEX_LOCK( ctx_mutex );
                    ctx.running = false;
                    MUTEX_UNLOCK( ctx_mutex );
                }
            return result;
        }

    MUTEX_LOCK( ctx_mutex );
    result = ctx.running;
    MUTEX_UNLOCK( ctx_mutex );

    return result;
}

// Process N CPU cycles (4 ticks/cycle: timers+PPU+APU per tick, DMA post-cycle)
void
AddEmulatorCycles( u32 cpu_cycles )
{
    MUTEX_LOCK( ctx_mutex );
    for( u32 i = 0; i < cpu_cycles; ++i )
        {
            for( int n = 0; n < 4; ++n )
                {
                    ++ctx.ticks;
                    //TickTimer();
                    //TickPPU();
                }

            //TickDMA();
        }
    MUTEX_UNLOCK( ctx_mutex );
}

// Get the current running state of the emulation
bool
IsEmulatorRunning( void )
{
    bool running;
    MUTEX_LOCK( ctx_mutex );
    running = ctx.running;
    MUTEX_UNLOCK( ctx_mutex );
    return running;
}

// Get the emulator context
DEPRECATED EmuContext *
GetEmulatorContext( void )
{
    // WARN: This function is problematic for thread safety
    return &ctx;
}

// Pause the emulation
void
PauseEmulator( void )
{
    MUTEX_LOCK( ctx_mutex );
    ctx.paused = true;
    MUTEX_UNLOCK( ctx_mutex );
}

// Resume the emulation
void
ResumeEmulator( void )
{
    MUTEX_LOCK( ctx_mutex );
    ctx.paused = false;
    MUTEX_UNLOCK( ctx_mutex );
}

// Stop the emulation and clean up resources
void
StopEmulator( void )
{
    MUTEX_LOCK( ctx_mutex );
    ctx.running = false;
    MUTEX_UNLOCK( ctx_mutex );

    THREAD_JOIN( cpu_thread );

    // Clean up mutex
    MUTEX_DESTROY( ctx_mutex );
}
