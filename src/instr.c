#include "camecore/camecore.h"
#include "camecore/utils.h"

Instruction instructions[0x100] = {
    [0x00] = { INS_NOP, AM_IMP },

    [0x05] = { INS_DEC, AM_R, RT_B },

    [0x0E] = { INS_LD, AM_R_D8, RT_C },

    [0xAF] = { INS_XOR, AM_R, RT_A },

    [0xC3] = { INS_JP, AM_D16 },

    [0xF3] = { INS_DI }
};

char * inst_lookup[] = { "<NONE>", "NOP",     "LD",      "INC",      "DEC",     "RLCA",    "ADD",     "RRCA",
                         "STOP",   "RLA",     "JR",      "RRA",      "DAA",     "CPL",     "SCF",     "CCF",
                         "HALT",   "ADC",     "SUB",     "SBC",      "AND",     "XOR",     "OR",      "CP",
                         "POP",    "JP",      "PUSH",    "RET",      "CB",      "CALL",    "RETI",    "LDH",
                         "JPHL",   "DI",      "EI",      "RST",      "INS_ERR", "INS_RLC", "INS_RRC", "INS_RL",
                         "INS_RR", "INS_SLA", "INS_SRA", "INS_SWAP", "INS_SRL", "INS_BIT", "INS_RES", "INS_SET" };

char *
GetInstructionName( InsType t )
{
    return inst_lookup[t];
}
