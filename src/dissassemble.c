#include "camecore/camecore.h"
#include "camecore/utils.h"

#include <stdio.h>

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
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the instruction name by given `InsType`
char *
GetInstructionName( InsType t )
{
    return INS_LOOKUP[t];
}

void
Disassemble( CPUContext * ctx, char * str )
{
    Instruction * inst      = ctx->inst_state.cur_inst;
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
                sprintf( str, "%s %s,$%04X", inst_name, RT_LOOKUP[inst->primary_reg], ctx->inst_state.fetched_data );
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
                         ctx->inst_state.fetched_data & 0xFF );
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
                sprintf( str, "%s $%02X,%s", inst_name, ReadBus( ctx->regs.pc - 1 ), RT_LOOKUP[inst->secondary_reg] );
                break;

            case AM_HL_SPR:
                // HL register to stack pointer plus offset
                sprintf( str, "%s (%s),SP+%d", inst_name, RT_LOOKUP[inst->primary_reg],
                         ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_D8:
                // 8-bit immediate data
                sprintf( str, "%s $%02X", inst_name, ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_D16:
                // 16-bit immediate data
                sprintf( str, "%s $%04X", inst_name, ctx->inst_state.fetched_data );
                break;

            case AM_MR_D8:
                // Memory address in register with 8-bit immediate data
                sprintf( str, "%s (%s),$%02X", inst_name, RT_LOOKUP[inst->primary_reg],
                         ctx->inst_state.fetched_data & 0xFF );
                break;

            case AM_A16_R:
                // 16-bit address to register
                sprintf( str, "%s ($%04X),%s", inst_name, ctx->inst_state.fetched_data,
                         RT_LOOKUP[inst->secondary_reg] );
                break;

            default: LOG( LOG_FATAL, "INVALID ADDRESSING MODE: %d", inst->addr_mode );
        }
}
