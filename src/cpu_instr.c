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
// TODO: Add the missing OpCodes
// TODO: Review the OpCodes Regs and Conditions
static Instruction instructions[0x100] = {
    // 0x0X
    [0x00] = { INS_NOP,   AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},
    [0x01] = {  INS_LD, AM_R_D16,   RT_BC, RT_NONE, CT_NONE, 0x00},
    [0x02] = {  INS_LD,  AM_MR_R,   RT_BC,    RT_A, CT_NONE, 0x00},

    [0x05] = { INS_DEC,     AM_R,    RT_B, RT_NONE, CT_NONE, 0x00},
    [0x06] = {  INS_LD,  AM_R_D8,    RT_B, RT_NONE, CT_NONE, 0x00},

    [0x08] = {  INS_LD, AM_A16_R, RT_NONE,   RT_SP, CT_NONE, 0x00},

    [0x0A] = {  INS_LD,  AM_R_MR,    RT_A,   RT_BC, CT_NONE, 0x00},

    [0x0E] = {  INS_LD,  AM_R_D8,    RT_C, RT_NONE, CT_NONE, 0x00},

    // 0x1X
    [0x11] = {  INS_LD, AM_R_D16,   RT_DE, RT_NONE, CT_NONE, 0x00},
    [0x12] = {  INS_LD,  AM_MR_R,   RT_DE,    RT_A, CT_NONE, 0x00},
    [0x15] = { INS_DEC,     AM_R,    RT_D, RT_NONE, CT_NONE, 0x00},
    [0x16] = {  INS_LD,  AM_R_D8,    RT_D, RT_NONE, CT_NONE, 0x00},
    [0x1A] = {  INS_LD,  AM_R_MR,    RT_A,   RT_DE, CT_NONE, 0x00},
    [0x1E] = {  INS_LD,  AM_R_D8,    RT_E, RT_NONE, CT_NONE, 0x00},

    // 0x2X
    [0x21] = {  INS_LD, AM_R_D16,   RT_HL, RT_NONE, CT_NONE, 0x00},
    [0x22] = {  INS_LD, AM_HLI_R,   RT_HL,    RT_A, CT_NONE, 0x00},
    [0x25] = { INS_DEC,     AM_R,    RT_H, RT_NONE, CT_NONE, 0x00},
    [0x26] = {  INS_LD,  AM_R_D8,    RT_H, RT_NONE, CT_NONE, 0x00},
    [0x2A] = {  INS_LD, AM_R_HLI,    RT_A,   RT_HL, CT_NONE, 0x00},
    [0x2E] = {  INS_LD,  AM_R_D8,    RT_L, RT_NONE, CT_NONE, 0x00},

    // 0x3X
    [0x31] = {  INS_LD, AM_R_D16,   RT_SP, RT_NONE, CT_NONE, 0x00},
    [0x32] = {  INS_LD, AM_HLD_R,   RT_HL,    RT_A, CT_NONE, 0x00},
    [0x35] = { INS_DEC,     AM_R,   RT_HL, RT_NONE, CT_NONE, 0x00},
    [0x36] = {  INS_LD, AM_MR_D8,   RT_HL, RT_NONE, CT_NONE, 0x00},
    [0x3A] = {  INS_LD, AM_R_HLD,    RT_A,   RT_HL, CT_NONE, 0x00},
    [0x3E] = {  INS_LD,  AM_R_D8,    RT_A, RT_NONE, CT_NONE, 0x00},

    // 0x4X
    [0x40] = {  INS_LD,   AM_R_R,    RT_B,    RT_B, CT_NONE, 0x00},
    [0x41] = {  INS_LD,   AM_R_R,    RT_B,    RT_C, CT_NONE, 0x00},
    [0x42] = {  INS_LD,   AM_R_R,    RT_B,    RT_D, CT_NONE, 0x00},
    [0x43] = {  INS_LD,   AM_R_R,    RT_B,    RT_E, CT_NONE, 0x00},
    [0x44] = {  INS_LD,   AM_R_R,    RT_B,    RT_H, CT_NONE, 0x00},
    [0x45] = {  INS_LD,   AM_R_R,    RT_B,    RT_L, CT_NONE, 0x00},
    [0x46] = {  INS_LD,  AM_R_MR,    RT_B,   RT_HL, CT_NONE, 0x00},
    [0x47] = {  INS_LD,   AM_R_R,    RT_B,    RT_A, CT_NONE, 0x00},
    [0x48] = {  INS_LD,   AM_R_R,    RT_C,    RT_B, CT_NONE, 0x00},
    [0x49] = {  INS_LD,   AM_R_R,    RT_C,    RT_C, CT_NONE, 0x00},
    [0x4A] = {  INS_LD,   AM_R_R,    RT_C,    RT_D, CT_NONE, 0x00},
    [0x4B] = {  INS_LD,   AM_R_R,    RT_C,    RT_E, CT_NONE, 0x00},
    [0x4C] = {  INS_LD,   AM_R_R,    RT_C,    RT_H, CT_NONE, 0x00},
    [0x4D] = {  INS_LD,   AM_R_R,    RT_C,    RT_L, CT_NONE, 0x00},
    [0x4E] = {  INS_LD,  AM_R_MR,    RT_C,   RT_HL, CT_NONE, 0x00},
    [0x4F] = {  INS_LD,   AM_R_R,    RT_C,    RT_A, CT_NONE, 0x00},

    // 0x5X
    [0x50] = {  INS_LD,   AM_R_R,    RT_D,    RT_B, CT_NONE, 0x00},
    [0x51] = {  INS_LD,   AM_R_R,    RT_D,    RT_C, CT_NONE, 0x00},
    [0x52] = {  INS_LD,   AM_R_R,    RT_D,    RT_D, CT_NONE, 0x00},
    [0x53] = {  INS_LD,   AM_R_R,    RT_D,    RT_E, CT_NONE, 0x00},
    [0x54] = {  INS_LD,   AM_R_R,    RT_D,    RT_H, CT_NONE, 0x00},
    [0x55] = {  INS_LD,   AM_R_R,    RT_D,    RT_L, CT_NONE, 0x00},
    [0x56] = {  INS_LD,  AM_R_MR,    RT_D,   RT_HL, CT_NONE, 0x00},
    [0x57] = {  INS_LD,   AM_R_R,    RT_D,    RT_A, CT_NONE, 0x00},
    [0x58] = {  INS_LD,   AM_R_R,    RT_E,    RT_B, CT_NONE, 0x00},
    [0x59] = {  INS_LD,   AM_R_R,    RT_E,    RT_C, CT_NONE, 0x00},
    [0x5A] = {  INS_LD,   AM_R_R,    RT_E,    RT_D, CT_NONE, 0x00},
    [0x5B] = {  INS_LD,   AM_R_R,    RT_E,    RT_E, CT_NONE, 0x00},
    [0x5C] = {  INS_LD,   AM_R_R,    RT_E,    RT_H, CT_NONE, 0x00},
    [0x5D] = {  INS_LD,   AM_R_R,    RT_E,    RT_L, CT_NONE, 0x00},
    [0x5E] = {  INS_LD,  AM_R_MR,    RT_E,   RT_HL, CT_NONE, 0x00},
    [0x5F] = {  INS_LD,   AM_R_R,    RT_E,    RT_A, CT_NONE, 0x00},

    // 0x6X
    [0x60] = {  INS_LD,   AM_R_R,    RT_H,    RT_B, CT_NONE, 0x00},
    [0x61] = {  INS_LD,   AM_R_R,    RT_H,    RT_C, CT_NONE, 0x00},
    [0x62] = {  INS_LD,   AM_R_R,    RT_H,    RT_D, CT_NONE, 0x00},
    [0x63] = {  INS_LD,   AM_R_R,    RT_H,    RT_E, CT_NONE, 0x00},
    [0x64] = {  INS_LD,   AM_R_R,    RT_H,    RT_H, CT_NONE, 0x00},
    [0x65] = {  INS_LD,   AM_R_R,    RT_H,    RT_L, CT_NONE, 0x00},
    [0x66] = {  INS_LD,  AM_R_MR,    RT_H,   RT_HL, CT_NONE, 0x00},
    [0x67] = {  INS_LD,   AM_R_R,    RT_H,    RT_A, CT_NONE, 0x00},
    [0x68] = {  INS_LD,   AM_R_R,    RT_L,    RT_B, CT_NONE, 0x00},
    [0x69] = {  INS_LD,   AM_R_R,    RT_L,    RT_C, CT_NONE, 0x00},
    [0x6A] = {  INS_LD,   AM_R_R,    RT_L,    RT_D, CT_NONE, 0x00},
    [0x6B] = {  INS_LD,   AM_R_R,    RT_L,    RT_E, CT_NONE, 0x00},
    [0x6C] = {  INS_LD,   AM_R_R,    RT_L,    RT_H, CT_NONE, 0x00},
    [0x6D] = {  INS_LD,   AM_R_R,    RT_L,    RT_L, CT_NONE, 0x00},
    [0x6E] = {  INS_LD,  AM_R_MR,    RT_L,   RT_HL, CT_NONE, 0x00},
    [0x6F] = {  INS_LD,   AM_R_R,    RT_L,    RT_A, CT_NONE, 0x00},

    // 0x7X
    [0x70] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_B, CT_NONE, 0x00},
    [0x71] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_C, CT_NONE, 0x00},
    [0x72] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_D, CT_NONE, 0x00},
    [0x73] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_E, CT_NONE, 0x00},
    [0x74] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_H, CT_NONE, 0x00},
    [0x75] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_L, CT_NONE, 0x00},
    [0x76] = {INS_HALT,   AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},
    [0x77] = {  INS_LD,  AM_MR_R,   RT_HL,    RT_A, CT_NONE, 0x00},
    [0x78] = {  INS_LD,   AM_R_R,    RT_A,    RT_B, CT_NONE, 0x00},
    [0x79] = {  INS_LD,   AM_R_R,    RT_A,    RT_C, CT_NONE, 0x00},
    [0x7A] = {  INS_LD,   AM_R_R,    RT_A,    RT_D, CT_NONE, 0x00},
    [0x7B] = {  INS_LD,   AM_R_R,    RT_A,    RT_E, CT_NONE, 0x00},
    [0x7C] = {  INS_LD,   AM_R_R,    RT_A,    RT_H, CT_NONE, 0x00},
    [0x7D] = {  INS_LD,   AM_R_R,    RT_A,    RT_L, CT_NONE, 0x00},
    [0x7E] = {  INS_LD,  AM_R_MR,    RT_A,   RT_HL, CT_NONE, 0x00},
    [0x7F] = {  INS_LD,   AM_R_R,    RT_A,    RT_A, CT_NONE, 0x00},

    [0xAF] = { INS_XOR,     AM_R,    RT_A, RT_NONE, CT_NONE, 0x00},
    [0xC3] = {  INS_JP,   AM_D16, RT_NONE, RT_NONE, CT_NONE, 0x00},

    // 0xEX
    [0xE0] = { INS_LDH,  AM_A8_R, RT_NONE,    RT_A, CT_NONE, 0x00},
    [0xE2] = {  INS_LD,  AM_MR_R,    RT_C,    RT_A, CT_NONE, 0x00},
    [0xEA] = {  INS_LD, AM_A16_R, RT_NONE,    RT_A, CT_NONE, 0x00},

    // 0xFX
    [0xF0] = { INS_LDH,  AM_R_A8,    RT_A, RT_NONE, CT_NONE, 0x00},
    [0xF2] = {  INS_LD,  AM_R_MR,    RT_A,    RT_C, CT_NONE, 0x00},
    [0xF3] = {  INS_DI,   AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},
    [0xFA] = {  INS_LD, AM_R_A16,    RT_A, RT_NONE, CT_NONE, 0x00},
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
