#include "camecore/camecore.h"
#include "camecore/utils.h"

typedef struct EmuContext
{
    bool paused;
    bool running;
    bool die;
    u64  ticks;
} EmuContext;

extern void CPUInit( void );

void
Init( void )
{
    LOG( LOG_INFO, "Initializing CameCore %s", CAMECORE_VERSION );

    CPUInit();
}
