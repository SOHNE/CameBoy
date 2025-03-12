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
// ...

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
Disassemble( CPUContext * cpu_ctx, char * str )
{
    Instruction * inst      = cpu_ctx->inst_state.cur_inst;
    const char *  inst_name = GetInstructionName( inst->type );

    // Format the instruction based on its addressing mode
    switch( inst->addr_mode )
        {
            case AM_IMP:
                // Implicit addressing mode - just the instruction name
                sprintf( str, "%s", inst_name );
                break;

            case AM_R_D16:
            case AM_R_A16:
                // Register with 16-bit immediate data/address
                sprintf( str, "%s %s,$%04X", inst_name, RT_LOOKUP[inst->primary_reg], cpu_ctx->inst_state.fetched_data );
                break;

            case AM_R:
                // Register operand only
                sprintf( str, "%s %s", inst_name, RT_LOOKUP[inst->primary_reg] );
                break;

            case AM_R_R:
                // Register to register
                sprintf( str, "%s %s,%s", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_MR_R:
                // Memory address in register to register
                sprintf( str, "%s (%s),%s", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_MR:
                // Memory address in register
                sprintf( str, "%s (%s)", inst_name, RT_LOOKUP[inst->primary_reg] );
                break;

            case AM_R_MR:
                // Register to memory address in register
                sprintf( str, "%s %s,(%s)", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_R_D8:
            case AM_R_A8:
                // Register with 8-bit immediate data/address
                sprintf( str, "%s %s,$%02X", inst_name, RT_LOOKUP[inst->primary_reg],
                         cpu_ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_R_HLI:
                // Register to memory address with post-increment
                sprintf( str, "%s %s,(%s+)", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_R_HLD:
                // Register to memory address with post-decrement
                sprintf( str, "%s %s,(%s-)", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_HLI_R:
                // Memory address with post-increment to register
                sprintf( str, "%s (%s+),%s", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_HLD_R:
                // Memory address with post-decrement to register
                sprintf( str, "%s (%s-),%s", inst_name, RT_LOOKUP[inst->primary_reg], RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_A8_R:
                // 8-bit address to register
                sprintf( str, "%s $%02X,%s", inst_name, ReadBus( cpu_ctx->regs.pc - 1 ), RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_HL_SPR:
                // HL register to stack pointer plus offset
                sprintf( str, "%s (%s),SP+%d", inst_name, RT_LOOKUP[inst->primary_reg],
                         cpu_ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_D8:
                // 8-bit immediate data
                sprintf( str, "%s $%02X", inst_name, cpu_ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_D16:
                // 16-bit immediate data
                sprintf( str, "%s $%04X", inst_name, cpu_ctx->inst_state.fetched_data );
                break;

            case AM_MR_D8:
                // Memory address in register with 8-bit immediate data
                sprintf( str, "%s (%s),$%02X", inst_name, RT_LOOKUP[inst->primary_reg],
                         cpu_ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_A16_R:
                // 16-bit address to register
                sprintf( str, "%s ($%04X),%s", inst_name, cpu_ctx->inst_state.fetched_data,
                         RT_LOOKUP[inst->secondary_reg] );
                break;

            default: LOG( LOG_FATAL, "INVALID ADDRESSING MODE: %d", inst->addr_mode );
        }
}
