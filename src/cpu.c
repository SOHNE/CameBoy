#include "camecore/camecore.h"
#include "camecore/utils.h"

/**
 * @struct CPURegisters
 * @brief Represents the Game Boy CPU registers
 *
 * The SM83 (Game Boy™ CPU) uses 8-bit registers that can also be accessed as one 16-bit.
 *
 * Overview:
 * 16-bit   Hi   Lo   Name/Function
 * ---------------------------------
 * AF       A    -    Accumulator & Flags
 * BC       B    C    BC
 * DE       D    E    DE
 * HL       H    L    HL
 * SP       -    -    Stack Pointer            - Points to the current top of the stack
 * PC       -    -    Program Counter/Pointer  - Points to the next instruction to be executed
 *
 * The F (Flags):
 * Bit   Name   Explanation
 * -------------------------
 * 7     z      Zero flag              - set when an operation results in zero
 * 6     n      Subtraction flag (BCD) - set if the last operation was a subtraction
 * 5     h      Half Carry flag (BCD)  - used in BCD (binary-coded decimal)
 * 4     c      Carry flag             - set when an operation produces a carry or borrow
 */
typedef struct CPURegisters
{
    u8  a;  /**< Accumulator for arithmetic and logic operations */
    u8  f;  /**< Flags register: z (Zero), n (Subtract), h (Half Carry), c (Carry) */
    u8  b;  /**< B register (high byte of BC) */
    u8  c;  /**< C register (low byte of BC) */
    u8  d;  /**< D register (high byte of DE) */
    u8  e;  /**< E register (low byte of DE) */
    u8  h;  /**< H register (high byte of HL) */
    u8  l;  /**< L register (low byte of HL) */
    u16 pc; /**< Program Counter: points to the next instruction */
    u16 sp; /**< Stack Pointer: points to the current top of the stack */

} CPURegisters;

/**
 * @struct CPUContext
 * @brief Represents the Game Boy™ CPU execution context
 *
 * The CPUContext structure maintains both the CPU register state and additional
 * execution context.
 *
 * The context is organized into logical components:
 * - CPU registers (standard SM83 register set)
 * - Instruction state (current execution context)
 * - Interrupt control (interrupt flags and enable registers)
 * - CPU status flags (operational mode indicators)
 *
 * @see CPURegisters for details on the Game Boy CPU register structure
 */
typedef struct CPUContext
{
    CPURegisters regs; /**< All CPU registers (A, F, B, C, D, E, H, L, PC, SP) */

    /**
     * @struct InstructionState
     * @brief Current instruction execution state
     *
     * Tracks data and addressing information for the current instruction cycle.
     * These fields maintain the state between different phases of instruction
     * execution (FETCH, DECODE, EXECUTE).
     */
    struct InstructionState
    {
        u16  fetch_data;  /**< Data fetched for the current instruction */
        u16  mem_dest;    /**< Memory destination address for the current operation */
        bool dest_is_mem; /**< Flag indicating if destination is memory (true) or register (false) */
        u8   cur_opcode;  /**< Current instruction opcode being executed */
    } inst_state;

    /**
     * @struct InterruptState
     * @brief Interrupt control and status flags
     *
     * Manages the Game Boy's interrupt system state, including:
     * - Master enable flag (IME)
     * - Pending IME activation
     * - IE register ($FFFF) - individual interrupt enable bits
     * - IF register ($FF0F) - interrupt request flags
     */
    struct InterruptState
    {
        bool ime;           /**< Interrupt Master Enable flag; controls global interrupt handling */
        bool ime_scheduled; /**< Flag for delayed IME activation (from EI instruction) */
        u8   ie_reg;        /**< Interrupt Enable register ($FFFF); enables specific interrupts */
        u8   if_reg;        /**< Interrupt Flag register ($FF0F); indicates pending interrupts */
    } int_state;

    /**
     * @struct CPUStatus
     * @brief CPU operational status flags
     *
     * Indicates special CPU states that affect instruction execution flow.
     * These flags control the behavior of the CPU execution loop and are
     * used for both normal operation and debugging purposes.
     */
    struct CPUStatus
    {
        bool halted;   /**< CPU HALT state flag; true when CPU is halted waiting for interrupt */
        bool stepping; /**< Debug mode flag; true when in single-step execution */
        bool stop;     /**< STOP mode flag; true when CPU is in low-power STOP mode */
    } status;

} CPUContext;

static CPUContext ctx = { 0 };

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------

void CPUInit( void );

void
CPUInit( void )
{
    ctx.regs.pc = 0x100;
    ctx.regs.a  = 0x01;
}

