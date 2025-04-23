/****************************** CameCore *********************************
 *
 * Module: Disassembler
 *
 * Handles conversion of machine code instructions into human-readable
 * assembly mnemonics with proper formatting of operands and addressing modes.
 *
 * Key Features:
 * - Instruction name lookup table
 * - Register name translation
 * - Addressing mode-aware disassembly formatting
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
// ...

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
static char * INS_LOOKUP[] = {
    "<NONE>", "NOP",     "LD",      "INC",      "DEC",     "RLCA",    "ADD",     "RRCA",    "STOP",    "RLA",
    "JR",     "RRA",     "DAA",     "CPL",      "SCF",     "CCF",     "HALT",    "ADC",     "SUB",     "SBC",
    "AND",    "XOR",     "OR",      "CP",       "POP",     "JP",      "PUSH",    "RET",     "CB",      "CALL",
    "RETI",   "LDH",     "JPHL",    "DI",       "EI",      "RST",     "INS_ERR", "INS_RLC", "INS_RRC", "INS_RL",
    "INS_RR", "INS_SLA", "INS_SRA", "INS_SWAP", "INS_SRL", "INS_BIT", "INS_RES", "INS_SET",
};

static char * RT_LOOKUP[] = {
    "<NONE>", "A", "F", "B", "C", "D", "E", "H", "L", "AF", "BC", "DE", "HL", "SP", "PC",
};

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
char * GetInstructionName( InsType t );
void   FormatInstructionBytes( CPUContext * cpu_ctx, char * bytes_str, size_t bytes_str_size );
void   Disassemble( CPUContext * cpu_ctx, char * str, size_t str_size );

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
char *
GetInstructionName( InsType t )
{
    return INS_LOOKUP[t];
}

void
FormatInstructionBytes( CPUContext * cpu_ctx, char * bytes_str, size_t bytes_str_size )
{
    char str[20];
    u16  pc     = cpu_ctx->regs.pc;
    u8   size   = cpu_ctx->inst_state.cur_inst->size;
    u8   opcode = cpu_ctx->inst_state.cur_opcode;

    if( size == 1 )
        {
            snprintf( str, sizeof( str ), "%02X", opcode );
        }
    else if( size == 2 )
        {
            snprintf( str, sizeof( str ), "%02X %02X", opcode, ReadBus( pc + 1 ) );
        }
    else if( size == 3 )
        {
            snprintf( str, sizeof( str ), "%02X %02X %02X", opcode, ReadBus( pc + 1 ), ReadBus( pc + 2 ) );
        }
    else
        {
            snprintf( str, sizeof( str ), "%02X ??", opcode );
        }

    snprintf( bytes_str, bytes_str_size, "%-8s", str );
}

void
Disassemble( CPUContext * cpu_ctx, char * str, size_t str_size )
{
    char          bytes_str[16];
    Instruction * inst      = cpu_ctx->inst_state.cur_inst;
    const char *  inst_name = GetInstructionName( inst->type );
    char          instruction[256];
    u16           data     = cpu_ctx->inst_state.fetched_data;
    u8            data_low = LOW_BYTE( data );

    switch( inst->addr_mode )
        {
            case AM_IMP: snprintf( instruction, sizeof( instruction ), "%s", inst_name ); break;
            case AM_R_D16:
            case AM_R_A16:
                snprintf( instruction, sizeof( instruction ), "%s %s,$%04X", inst_name, RT_LOOKUP[inst->primary_reg],
                          data );
                break;
            case AM_R:
                snprintf( instruction, sizeof( instruction ), "%s %s", inst_name, RT_LOOKUP[inst->primary_reg] );
                break;
            case AM_R_R:
                snprintf( instruction, sizeof( instruction ), "%s %s,%s", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_MR_R:
                snprintf( instruction, sizeof( instruction ), "%s (%s),%s", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_MR:
                snprintf( instruction, sizeof( instruction ), "%s (%s)", inst_name, RT_LOOKUP[inst->primary_reg] );
                break;
            case AM_R_MR:
                snprintf( instruction, sizeof( instruction ), "%s %s,(%s)", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_R_D8:
            case AM_R_A8:
                snprintf( instruction, sizeof( instruction ), "%s %s,$%02X", inst_name, RT_LOOKUP[inst->primary_reg],
                          data_low );
                break;
            case AM_R_HLI:
                snprintf( instruction, sizeof( instruction ), "%s %s,(%s+)", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_R_HLD:
                snprintf( instruction, sizeof( instruction ), "%s %s,(%s-)", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_HLI_R:
                snprintf( instruction, sizeof( instruction ), "%s (%s+),%s", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_HLD_R:
                snprintf( instruction, sizeof( instruction ), "%s (%s-),%s", inst_name, RT_LOOKUP[inst->primary_reg],
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_A8_R:
                snprintf( instruction, sizeof( instruction ), "%s $%02X,%s", inst_name, ReadBus( cpu_ctx->regs.pc - 1 ),
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_HL_SPR:
                snprintf( instruction, sizeof( instruction ), "%s (%s),SP+%d", inst_name, RT_LOOKUP[inst->primary_reg],
                          data_low );
                break;
            case AM_D8:  snprintf( instruction, sizeof( instruction ), "%s $%02X", inst_name, data_low ); break;
            case AM_D16: snprintf( instruction, sizeof( instruction ), "%s $%04X", inst_name, data ); break;
            case AM_D16_R:
                snprintf( instruction, sizeof( instruction ), "%s $%04X,%s", inst_name, data,
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            case AM_MR_D8:
                snprintf( instruction, sizeof( instruction ), "%s (%s),$%02X", inst_name, RT_LOOKUP[inst->primary_reg],
                          data_low );
                break;
            case AM_A16_R:
                snprintf( instruction, sizeof( instruction ), "%s ($%04X),%s", inst_name, data,
                          RT_LOOKUP[inst->secondary_reg] );
                break;
            default: LOG( LOG_FATAL, "INVALID ADDRESSING MODE: %d", inst->addr_mode );
        }

    FormatInstructionBytes( cpu_ctx, bytes_str, sizeof( bytes_str ) );
    snprintf( str, str_size, "%-16s %-12s", instruction, bytes_str );
}

