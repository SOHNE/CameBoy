#include "camecore/camecore.h"

//----------------------------------------------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------------------------------------------
static Instruction instructions[0x100] = {
    [0x00] = {INS_NOP,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},

    [0x05] = {INS_DEC,    AM_R,    RT_B, RT_NONE, CT_NONE, 0x00},

    [0x0E] = { INS_LD, AM_R_D8,    RT_C, RT_NONE, CT_NONE, 0x00},

    [0xAF] = {INS_XOR,    AM_R,    RT_A, RT_NONE, CT_NONE, 0x00},

    [0xC3] = { INS_JP,  AM_D16, RT_NONE, RT_NONE, CT_NONE, 0x00},

    [0xF3] = { INS_DI,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00}
};

static char * inst_lookup[] = {
    "<NONE>", "NOP",     "LD",      "INC",      "DEC",     "RLCA",    "ADD",     "RRCA",    "STOP",    "RLA",
    "JR",     "RRA",     "DAA",     "CPL",      "SCF",     "CCF",     "HALT",    "ADC",     "SUB",     "SBC",
    "AND",    "XOR",     "OR",      "CP",       "POP",     "JP",      "PUSH",    "RET",     "CB",      "CALL",
    "RETI",   "LDH",     "JPHL",    "DI",       "EI",      "RST",     "INS_ERR", "INS_RLC", "INS_RRC", "INS_RL",
    "INS_RR", "INS_SLA", "INS_SRA", "INS_SWAP", "INS_SRL", "INS_BIT", "INS_RES", "INS_SET" };

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Retrieve the `Instruction` by given `OpCode`
Instruction *
GetInstructionByOpCode( u8 opcode )
{
    return &instructions[opcode];
}

// Retrieve the instruction name by given `InsType`
char *
GetInstructionName( InsType t )
{
    return inst_lookup[t];
}
