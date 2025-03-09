/****************************** CameCore *********************************
 *
 * Module: CPU Instruction
 *
 * It defines the instruction lookup tables and associated functions:
 *   - An array of instruction definitions indexed by opcode.
 *   - A lookup table for instruction mnemonic strings.
 *   - Functions to retrieve instruction details by opcode.
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

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
Instruction * GetInstructionByOpCode( u8 opcode );
char *        GetInstructionName( InsType t );

//----------------------------------------------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// Initialize the instructions table indexed by opcode (0x00 to 0xFF)
static const Instruction instructions[0x100] = {
    /**           MNE,      AM,    1REG,    2REG,    COND,  ARGS */
    [0x00] = {INS_NOP,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},

    [0x05] = {INS_DEC,    AM_R,    RT_B, RT_NONE, CT_NONE, 0x00},

    [0x0E] = { INS_LD, AM_R_D8,    RT_C, RT_NONE, CT_NONE, 0x00},

    [0xAF] = {INS_XOR,    AM_R,    RT_A, RT_NONE, CT_NONE, 0x00},

    [0xC3] = { INS_JP,  AM_D16, RT_NONE, RT_NONE, CT_NONE, 0x00},

    [0xF3] = { INS_DI,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00}
};

static const char * inst_lookup[] = {
    "<NONE>", "NOP",     "LD",      "INC",      "DEC",     "RLCA",    "ADD",     "RRCA",    "STOP",    "RLA",
    "JR",     "RRA",     "DAA",     "CPL",      "SCF",     "CCF",     "HALT",    "ADC",     "SUB",     "SBC",
    "AND",    "XOR",     "OR",      "CP",       "POP",     "JP",      "PUSH",    "RET",     "CB",      "CALL",
    "RETI",   "LDH",     "JPHL",    "DI",       "EI",      "RST",     "INS_ERR", "INS_RLC", "INS_RRC", "INS_RL",
    "INS_RR", "INS_SLA", "INS_SRA", "INS_SWAP", "INS_SRL", "INS_BIT", "INS_RES", "INS_SET",
};

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
