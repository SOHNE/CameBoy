#include "camecore/camecore.h"

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
#define REVERSE( n ) ( ( ( (u16)( n ) & 0xFF00 ) >> 8 ) | ( ( (u16)( n ) & 0x00FF ) << 8 ) )

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
extern CPUContext cpu_ctx;

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the register data by the given `RegType`
u16
GetRegister( RegType rt )
{
    switch( rt )
        {
            case RT_A:  return cpu_ctx.regs.a;
            case RT_F:  return cpu_ctx.regs.f;
            case RT_B:  return cpu_ctx.regs.b;
            case RT_C:  return cpu_ctx.regs.c;
            case RT_D:  return cpu_ctx.regs.d;
            case RT_E:  return cpu_ctx.regs.e;
            case RT_H:  return cpu_ctx.regs.h;
            case RT_L:  return cpu_ctx.regs.l;

            case RT_AF: return REVERSE( *( (u16 *)&cpu_ctx.regs.a ) );
            case RT_BC: return REVERSE( *( (u16 *)&cpu_ctx.regs.b ) );
            case RT_DE: return REVERSE( *( (u16 *)&cpu_ctx.regs.d ) );
            case RT_HL: return REVERSE( *( (u16 *)&cpu_ctx.regs.h ) );

            case RT_PC: return cpu_ctx.regs.pc;
            case RT_SP: return cpu_ctx.regs.sp;
            default:    return 0;
        }
    return 0;
}
