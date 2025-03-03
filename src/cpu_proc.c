#include "camecore/camecore.h"
#include "camecore/utils.h"

static void
ProcNONE( CPUContext * ctx )
{
    UNUSED( ctx );
    LOG( LOG_FATAL, "INVALID INSTRUCTION" );
}

static void
ProcNOP( CPUContext * ctx )
{
    UNUSED( ctx );
}

static void
ProcDI( CPUContext * ctx )
{
    UNUSED( ctx );
}

static void
ProcLD( CPUContext * ctx )
{
    UNUSED( ctx );
}

void
CpuSetFlags( CPUContext * ctx, char z, char n, char h, char c )
{
    UNUSED( ctx );
    UNUSED( z );
    UNUSED( n );
    UNUSED( h );
    UNUSED( c );
}

static void
ProcXOR( CPUContext * ctx )
{
    UNUSED( ctx );
}

static bool
CheckCond( CPUContext * ctx )
{
    UNUSED( ctx );

    return false;
}

static void
ProcJP( CPUContext * ctx )
{
    UNUSED( ctx );
}

CPUInstructionProc
GetInstructionProcessor( InsType type )
{
    switch( type )
        {
            case INS_NONE: return ProcNONE;
            case INS_NOP:  return ProcNOP;
            case INS_LD:   return ProcLD;
            case INS_JP:   return ProcJP;
            case INS_DI:   return ProcDI;
            case INS_XOR:  return ProcXOR;

            default:       return ProcNONE;
        }
}
