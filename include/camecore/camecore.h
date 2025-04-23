/****************************** CameCore *********************************
 *
 * Version:    v0.0.1                   Author: Leandro Peres (@zschzen)
 * Released:   2025-03-07               License: zlib/libpng (OSI Approved)
 *
 * An ultra simple (and not complete) Game Boy™ emulator:
 * - Educational purposes and emulation fundamentals
 * - C99 development workflows
 *
 *                            QUICK START
 * ------------------------------------------------------------------------
 * #include "camecore/camecore.h"
 *
 * int main(void) {
 *     // Initialize the emulator system
 *     InitEmulator();
 *
 *     // Attempt to load a cartridge file (replace "path/to/cartridge.bin" with your file)
 *     if (!LoadCartridge("path/to/cartridge.bin")) {
 *         return 1;
 *     }
 *
 *     // Main emulation loop
 *     while (IsEmulatorRunning()) {
 *         // Execute one emulation step
 *         if (!StepEmulator()) {
 *             break;
 *         }
 *     }
 *
 *     return 0;
 * }
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *   wrote the original software. If you use this software in a product, an acknowledgment
 *   in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *   as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 *
 *************************************************************************/

#ifndef CAMECORE_H
#define CAMECORE_H

#include "camecore/version.h"
#include "ccapi.h"

#include <stdarg.h> /** va_list */
#include <stdint.h>
#include <stdlib.h>

// Ensure boolean type
#if ( defined( __STDC__ ) && __STDC_VERSION__ >= 199901L ) || ( defined( _MSC_VER ) && _MSC_VER >= 1800 )
#    include <stdbool.h>
#elif !defined( __cplusplus ) && !defined( bool )
typedef enum bool
{
    false = 0,
    true  = !false
} bool;
#    define CC_BOOL_TYPE
#endif

//----------------------------------------------------------------------------------------------------------------------
// Base types
//----------------------------------------------------------------------------------------------------------------------
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int64_t  i64;
typedef uint64_t u64;

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------
// C++ compatibility, preventing name mangling
#if defined( __cplusplus )
#    define CXX_GUARD_START                                                                                            \
        extern "C"                                                                                                     \
        {
#    define CXX_GUARD_END }
#else
#    define CXX_GUARD_START
#    define CXX_GUARD_END
#endif

#if defined( __cplusplus )
#    define CLITERAL( type ) type
#else
#    define CLITERAL( type ) ( type )
#endif

/// compound literals
#define UNUSED( x ) (void)( x )

/// No implementation
#define NO_IMPL()   LOG( LOG_FATAL, "Not implemented: %s (%s:%d)", __func__, __FILE__, __LINE__ )

/// Compiler attributes
#if defined( __GNUC__ ) || defined( __clang__ )
#    define NORETURN      __attribute__( ( noreturn ) )
#    define PACKED        __attribute__( ( packed ) )
#    define INLINE        inline __attribute__( ( always_inline ) )
#    define ALIGNED( x )  __attribute__( ( aligned( x ) ) ) // Align variables/structs
#    define DEPRECATED    __attribute__( ( deprecated ) )   // Mark as deprecated
#    define LIKELY( x )   __builtin_expect( !!( x ), 1 )    // Branch prediction hint
#    define UNLIKELY( x ) __builtin_expect( !!( x ), 0 )
#elif defined( _MSC_VER )
#    define NORETURN      __declspec( noreturn )
#    define PACKED        __pragma( pack( push, 1 ) )
#    define INLINE        __forceinline
#    define ALIGNED( x )  __declspec( align( x ) )
#    define DEPRECATED    __declspec( deprecated )
#    define LIKELY( x )   ( x )
#    define UNLIKELY( x ) ( x )
#else
#    define NORETURN
#    define PACKED
#    define INLINE inline
#    define ALIGNED( x )
#    define DEPRECATED
#    define LIKELY( x )   ( x )
#    define UNLIKELY( x ) ( x )
#endif

// Validations
#define IS_STR_VALID( str )       ( ( str ) != NULL && ( str )[0] != '\0' )

// Checks if value is within [low, high] (inclusive)
#define BETWEEN( val, low, high ) ( ( ( low ) <= ( val ) ) && ( ( val ) <= ( high ) ) )

// Direct bit operations
#define BIT( n )                  ( 0x01U << ( n ) )
#define BIT_SET( r, n )           ( ( r ) |= BIT( n ) )
#define BIT_CLEAR( r, n )         ( ( r ) &= ~BIT( n ) )
#define BIT_TOGGLE( r, n )        ( ( r ) ^= BIT( n ) )
#define BIT_CHECK( r, n )         ( ( r ) & BIT( n ) )
#define BIT_ASSIGN( r, n, v )     ( ( v ) ? BIT_SET( ( r ), ( n ) ) : BIT_CLEAR( ( r ), ( n ) ) )

#define LOW_BYTE( val )           ( (u8)( ( val ) & 0xFFU ) )               /**< Extract LSB as 8-bit unsigned */
#define HIGH_BYTE( val )          ( (u8)( ( (u16)( val ) >> 8 ) & 0xFFU ) ) /**< Extract MSB safely */
#define MAKE_WORD( high, low )    ( (u16)( ( (u16)LOW_BYTE( high ) << 8 ) | LOW_BYTE( low ) ) ) /**< Make a 16-bit word */

// Flags operation macros
#define FLAG_SET( n, f )          ( ( n ) |= ( f ) )
#define FLAG_CLEAR( n, f )        ( ( n ) &= ~( f ) )
#define FLAG_TOGGLE( n, f )       ( ( n ) ^= ( f ) )
#define FLAG_CHECK( n, f )        ( ( n ) & ( f ) )

// Flag bit positions
#define FLAG_Z_BIT                7 /**< Zero flag */
#define FLAG_N_BIT                6 /**< Subtract flag */
#define FLAG_H_BIT                5 /**< Half-carry flag */
#define FLAG_C_BIT                4 /**< Carry flag */

// Registers Flag bit masks
#define FLAG_Z                    BIT( FLAG_Z_BIT )
#define FLAG_N                    BIT( FLAG_N_BIT )
#define FLAG_H                    BIT( FLAG_H_BIT )
#define FLAG_C                    BIT( FLAG_C_BIT )

// Array
#define ARRAY_LEN( a )            ( sizeof( ( a ) ) / sizeof( ( a )[0] ) )
#define INDEX_VALID( i, a )       ( ( i ) >= 0 && (size_t)( i ) < ARRAY_LEN( a ) )

// Memory ranges
#define ROM_BANK_SIZE             0x4000 /**< Size of each ROM bank (16 KiB) */
#define ROM_BANK0_START           0x0000 /**< Start address of fixed ROM bank 0 */
#define ROM_BANK0_END             0x3FFF /**< End address of fixed ROM bank 0 */
#define ROM_BANKN_START           0x4000 /**< Start address of switchable ROM bank */
#define ROM_BANKN_END             0x7FFF /**< End address of switchable ROM bank */

#define VRAM_SIZE                 0x2000 /**< Total size of Video RAM (8 KiB) */
#define VRAM_START                0x8000 /**< Start address of VRAM */
#define VRAM_END                  0x9FFF /**< End address of VRAM */
#define VRAM_CHR_RAM_START        0x8000 /**< Start address of CHR RAM */
#define VRAM_CHR_RAM_END          0x97FF /**< End address of CHR RAM */
#define VRAM_BG_MAP1_START        0x9800 /**< Start address of Background Map 1 */
#define VRAM_BG_MAP1_END          0x9BFF /**< End address of Background Map 1 */
#define VRAM_BG_MAP2_START        0x9C00 /**< Start address of Background Map 2 */
#define VRAM_BG_MAP2_END          0x9FFF /**< End address of Background Map 2 */

#define EXTRAM_SIZE               0x2000 /**< Size of external cartridge RAM (8 KiB) */
#define EXTRAM_START              0xA000 /**< Start address of external RAM */
#define EXTRAM_END                0xBFFF /**< End address of external RAM */

#define WRAM_SIZE                 0x2000 /**< Total size of Work RAM (8 KiB) */
#define WRAM_START                0xC000 /**< Start address of Work RAM */
#define WRAM_END                  0xDFFF /**< End address of Work RAM */
#define WRAM_BANKN_START          0xD000 /**< Start address of switchable WRAM banks */
#define WRAM_BANKN_END            0xDFFF /**< End address of switchable WRAM banks */

#define ECHO_SIZE                 0x1E00 /**< Size of Echo RAM */
#define ECHO_START                0xE000 /**< Start address of Echo RAM (mirrors C000-DDFF) */
#define ECHO_END                  0xFDFF /**< End address of Echo RAM */

#define OAM_SIZE                  0xA0   /**< Size of Object Attribute Memory (OAM) */
#define OAM_START                 0xFE00 /**< Start address of OAM */
#define OAM_END                   0xFE9F /**< End address of OAM */

#define IO_SIZE                   0x80   /**< Size of I/O register space */
#define IO_START                  0xFF00 /**< Start address of I/O registers */
#define IO_END                    0xFF7F /**< End address of I/O registers */

#define HRAM_SIZE                 0x80   /**< Size of High RAM (HRAM) */
#define HRAM_START                0xFF80 /**< Start address of High RAM */
#define HRAM_END                  0xFFFE /**< End address of High RAM */

#define IE_REGISTER               0xFFFF /**< Address of the Interrupt Enable Register */

//----------------------------------------------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    LOG_ALL = 0, // Display all logs
    LOG_TRACE,   // Trace logging, intended for internal use only
    LOG_DEBUG,   // Debug logging, used for internal debugging, it should be disabled on release builds
    LOG_INFO,    // Info logging, used for program execution info
    LOG_WARNING, // Warning logging, used on recoverable failures
    LOG_ERROR,   // Error logging, used on unrecoverable failures
    LOG_FATAL,   // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    LOG_NONE     // Disable logging
} TraceLogLevel;

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

    // CB instructions
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

//----------------------------------------------------------------------------------------------------------------------
// Struct Definition
//----------------------------------------------------------------------------------------------------------------------
typedef struct EmuContext
{
    bool paused;
    bool running;
    bool die;
    u64  ticks;
} EmuContext;

/**
 * @brief Instruction structure
 *
 * Represents a decoded CPU instruction with its operands
 */
typedef struct Instruction
{
    InsType  type;
    AddrMode addr_mode;
    RegType  primary_reg;
    RegType  secondary_reg;
    CondType condition_type;
    u8       param;
    u8       cycles; // Duration in CPU cycles
    u8       size;   // Size in bytes
} Instruction;

/**
 * @brief Structure for CPU registers
 *
 * The SM83 (Game Boy™'s CPU) uses 8-bit registers that can also be accessed as 16-bit.
 *
 * Overview:
 * - Registers: A, F, B, C, D, E, H, L
 * - Register pairs: AF, BC, DE, HL
 * - Special registers: SP (Stack Pointer), PC (Program Counter)
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
 * @brief Structure for CPU execution context
 *
 * The CPUContext structure maintains the CPU state including registers,
 * instruction state, interrupt control, and CPU status flags.
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
        u16           fetched_data; /**< Data fetched for the current instruction */
        u16           mem_dest;     /**< Memory destination address for the current operation */
        bool          dest_is_mem;  /**< Flag indicating if destination is memory (true) or register (false) */
        u8            cur_opcode;   /**< Current instruction opcode being executed */
        Instruction * cur_inst;
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
    } interupt_state;

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

//----------------------------------------------------------------------------------------------------------------------
// Functions callbacks
//----------------------------------------------------------------------------------------------------------------------
typedef void ( *CPUInstructionProc )( CPUContext * /* ctx */ );
typedef void ( *TraceLogCallback )( int logLevel, const char * text, va_list args ); // Custom trace log

//----------------------------------------------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------------------------------------------

CXX_GUARD_START

// Core
//------------------------------------------------------------------
CCAPI void         InitEmulator( void );
CCAPI bool         StepEmulator( void );
CCAPI void         AddEmulatorCycles( u32 cpu_cycles );
CCAPI bool         IsEmulatorRunning( void );
CCAPI EmuContext * GetEmulatorContext( void );
CCAPI void         PauseEmulator( void );
CCAPI void         ResumeEmulator( void );
CCAPI void         StopEmulator( void );

// CPU
//------------------------------------------------------------------
CCAPI u16            ReadRegister( RegType rt );
CCAPI void           SetRegister( RegType rt, u16 val );
CCAPI CPURegisters * GetRegisters( void );

CCAPI void PushStack( u8 data );
CCAPI void PushStackWord( u16 data );
CCAPI u8   PopStack( void );
CCAPI u16  PopStackWord( void );

// Bus
//------------------------------------------------------------------
CCAPI u8   ReadBus( u16 addr );
CCAPI void WriteBus( u16 addr, u8 value );

CCAPI void WriteBusWord( u16 address, u16 value );
CCAPI u16  ReadBusWord( u16 address );

// RAM
//------------------------------------------------------------------
CCAPI u8   ReadWRAM( u16 addr );
CCAPI void WriteWRAM( u16 addr, u8 value );

CCAPI u8   ReadHRAM( u16 addr );
CCAPI void WriteHRAM( u16 addr, u8 value );

// Cart
//------------------------------------------------------------------
CCAPI bool LoadCartridge( char * cart );
CCAPI u8   ReadCartridge( u16 address );
CCAPI void WriteCartridge( u16 address, u8 value );

// IO
//------------------------------------------------------------------
CCAPI u8   ReadIO( u16 addr );
CCAPI void WriteIO( u16 addr, u8 value );

// Utils
//------------------------------------------------------------------
CCAPI void TraceLog( i32 logLevel, const char * text, ... );
CCAPI void SetLogLevel( i32 logLevel );

CCAPI u8 * LoadFileData( const char * filename, size_t * outBytesRead );
CCAPI bool SaveFileData( const char * filename, const u8 * data, size_t dataSize );

CCAPI void SetTraceLogCallback( TraceLogCallback callback ); // Set custom trace log

CXX_GUARD_END

#endif // !CAMECORE_H
