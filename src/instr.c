#include "camecore/camecore.h"
#include "camecore/utils.h"

// Addressing modes
typedef enum
{
    AM_IMP,   /**< Implicit addressing; no operand is specified in the instruction */
    AM_R_D16, /**< Register paired with a 16-bit immediate value (e.g., LD R, d16) */
    AM_R_R,   /**< Register-to-register operation (e.g., LD R, R) */
    AM_MR_R,  /**< Memory addressed by a register (typically HL) receives a value from a register (e.g., LD (HL), R) */
    AM_R,     /**< Single register operand; used when only one register is involved */
    AM_R_D8,  /**< Register paired with an 8-bit immediate value (e.g., LD R, d8) */
    AM_R_MR,  /**< Register receives a value from memory addressed by a register (e.g., LD R, (HL)) */
    AM_R_HLI, /**< Register receives a value from memory addressed by HL, then HL is incremented (post-increment addressing) */
    AM_R_HLD, /**< Register receives a value from memory addressed by HL, then HL is decremented (post-decrement addressing) */
    AM_HLI_R,  /**< Memory at address HL (before increment) gets a value from a register (write then post-increment) */
    AM_HLD_R,  /**< Memory at address HL (before decrement) gets a value from a register (write then post-decrement) */
    AM_R_A8,   /**< Register with an 8-bit immediate address (often used for zero-page addressing) */
    AM_A8_R,   /**< Memory at an 8-bit immediate address receives a value from a register */
    AM_HL_SPR, /**< HL register is set based on the Stack Pointer plus an offset (e.g., LD HL, SP+d8) */
    AM_D16,    /**< Direct 16-bit immediate data (typically an address literal) */
    AM_D8,     /**< Direct 8-bit immediate data */
    AM_D16_R,  /**< 16-bit immediate value used as an operand in register operations */
    AM_MR_D8,  /**< Memory (addressed by HL) receives an 8-bit immediate value (e.g., LD (HL), d8) */
    AM_MR,     /**< Memory register addressing mode; used when a memory location is accessed via a register */
    AM_A16_R,  /**< Memory at a 16-bit absolute address receives a value from a register (e.g., LD (a16), R) */
    AM_R_A16   /**< Register receives a value from a 16-bit absolute address (e.g., LD R, (a16)) */
} AddrMode;

// Register types
typedef enum
{
    RT_NONE, /**< No register; used as a placeholder */
    RT_A,    /**< Accumulator register */
    RT_F,    /**< Flags register (contains status flags) */
    RT_B,    /**< Register B */
    RT_C,    /**< Register C */
    RT_D,    /**< Register D */
    RT_E,    /**< Register E */
    RT_H,    /**< Register H */
    RT_L,    /**< Register L */
    RT_AF,   /**< Combined register pair: Accumulator and Flags (A and F) */
    RT_BC,   /**< Combined register pair: B and C */
    RT_DE,   /**< Combined register pair: D and E */
    RT_HL,   /**< Combined register pair: H and L (often used for addressing memory) */
    RT_SP,   /**< Stack Pointer */
    RT_PC    /**< Program Counter */
} RegType;

// Instruction types
typedef enum
{
    INS_NONE, /**< No instruction (placeholder) */
    INS_NOP,  /**< No operation */
    INS_LD,   /**< Load instruction; moves data between registers or between memory and a register */
    INS_INC,  /**< Increment instruction; increases a value by one */
    INS_DEC,  /**< Decrement instruction; decreases a value by one */
    INS_RLCA, /**< Rotate A left (circular rotation; bit 7 moves into carry flag) */
    INS_ADD,  /**< Addition operation */
    INS_RRCA, /**< Rotate A right (circular rotation; bit 0 moves into carry flag) */
    INS_STOP, /**< Stop the CPU (often used to enter low-power mode) */
    INS_RLA,  /**< Rotate A left through the carry flag */
    INS_JR,   /**< Relative jump; adds a signed offset to the program counter */
    INS_RRA,  /**< Rotate A right through the carry flag */
    INS_DAA,  /**< Decimal adjust accumulator for Binary Coded Decimal (BCD) arithmetic */
    INS_CPL,  /**< Complement the accumulator (bitwise NOT of A) */
    INS_SCF,  /**< Set the carry flag */
    INS_CCF,  /**< Complement (invert) the carry flag */
    INS_HALT, /**< Halt CPU execution until an interrupt occurs */
    INS_ADC,  /**< Add with carry flag */
    INS_SUB,  /**< Subtract operation */
    INS_SBC,  /**< Subtract with borrow (considering the carry flag) */
    INS_AND,  /**< Bitwise AND operation */
    INS_XOR,  /**< Bitwise XOR operation */
    INS_OR,   /**< Bitwise OR operation */
    INS_CP,   /**< Compare instruction; subtracts and sets flags without storing the result */
    INS_POP,  /**< Pop data from the stack into a register */
    INS_JP,   /**< Absolute jump to an address */
    INS_PUSH, /**< Push data from a register onto the stack */
    INS_RET,  /**< Return from a subroutine */
    INS_CB,   /**< Prefix for extended CB instructions (special operations on bits) */
    INS_CALL, /**< Call subroutine; jumps to an address and saves the return address */
    INS_RETI, /**< Return from an interrupt routine */
    INS_LDH,  /**< Load high; special Game Boy instruction for accessing high memory or I/O registers */
    INS_JPHL, /**< Jump to the address contained in HL */
    INS_DI,   /**< Disable interrupts */
    INS_EI,   /**< Enable interrupts */
    INS_RST,  /**< Restart; similar to a subroutine call to a fixed memory address */
    INS_ERR,  /**< Error or undefined instruction */

    // CB instructions (extended opcodes following the IN_CB prefix)
    INS_RLC,  /**< Rotate left circular (operates on bits) */
    INS_RRC,  /**< Rotate right circular (operates on bits) */
    INS_RL,   /**< Rotate left through the carry flag */
    INS_RR,   /**< Rotate right through the carry flag */
    INS_SLA,  /**< Arithmetic shift left (logical shift left) */
    INS_SRA,  /**< Arithmetic shift right (preserves the sign bit) */
    INS_SWAP, /**< Swap the upper and lower nibbles of a byte */
    INS_SRL,  /**< Logical shift right (zero-fill) */
    INS_BIT,  /**< Test a specific bit in a register or memory location */
    INS_RES,  /**< Reset (clear) a specific bit in a register or memory location */
    INS_SET   /**< Set a specific bit in a register or memory location */
} InsType;

// Condition types for conditional instructions
typedef enum
{
    CT_NONE, /**< No condition; the instruction always executes */
    CT_NZ,   /**< Not zero condition; executes if the zero flag is not set */
    CT_Z,    /**< Zero condition; executes if the zero flag is set */
    CT_NC,   /**< Not carry condition; executes if the carry flag is not set */
    CT_C     /**< Carry condition; executes if the carry flag is set */
} CondType;

typedef struct instruction
{
    InsType  type;
    AddrMode mode;
    RegType  reg_1;
    RegType  reg_2;
    CondType cond;
    u8       param;
} instruction;

instruction instructions[0x100] = {
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
