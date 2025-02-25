#ifndef GBCE_H
#define GBCE_H

#include "camecore/version.h"
#include "ccapi.h"

#include <stdint.h>
#include <stdlib.h>

//---------------------------------
// Base types
//---------------------------------
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int64_t  i64;
typedef uint64_t u64;

//---------------------------------
// Defines
//---------------------------------
#if defined( __cplusplus )
#    define CLITERAL( type ) type
#else
#    define CLITERAL( type ) ( type )
#endif

/// compound literals
#define UNUSED( x ) (void)( x )

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

//---------------------------------
// Bit operations
//---------------------------------
#define BIT( n )                  ( 0x1U << ( n ) )
#define BIT_SET( v, n )           ( ( v ) |= BIT( n ) )
#define BIT_CLEAR( v, n )         ( ( v ) &= ~BIT( n ) )
#define BIT_TEST( v, n )          ( !!( ( v ) & BIT( n ) ) )
#define BIT_TOGGLE( v, n )        ( ( v ) ^= BIT( n ) )     // Toggle bit
#define BIT_MASK( len )           ( ( 1U << ( len ) ) - 1 ) // Create a bit mask of 'len' bits
// 8-bit register manipulation
#define REG_GET( reg, mask )      ( ( reg ) & ( mask ) )
#define REG_SET( reg, mask, val ) ( ( reg ) = ( ( reg ) & ~( mask ) ) | ( ( val ) & ( mask ) ) )

//---------------------------------
// Emulation utilities
//---------------------------------
#define CC_MAX( a, b )            ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define CC_MIN( a, b )            ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define CC_CLAMP( x, lo, hi )     ( ( x ) < ( lo ) ? ( lo ) : ( ( x ) > ( hi ) ? ( hi ) : ( x ) ) )

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

// Basic structure
typedef struct
{
    uint8_t  memory[0x10000]; // 64KB memory
    uint8_t  rom[0x8000];     // 32KB cartridge ROM
    uint16_t pc;              // Program counter
} GameBoy;

/**
 * @struct RomHeader
 * @brief Game Boy cartridge header (0100-014Fh range)
 *
 * Defines ROM metadata including boot behavior, hardware requirements, and validation data.
 * Critical fields:
 * - Entry point instructions
 * - Nintendo logo bitmap (verified at boot)
 * - Title/manufacturer codes
 * - CGB/SGB compatibility flags
 * - Memory configuration (MBC type, ROM/RAM sizes)
 * - Checksums and regional codes
 *
 * @note Logo bytes (0104-0133h) must match Nintendo's bitmap or boot fails
 * @warning Header checksum (014Dh) must validate via 0134h-014Ch subtraction chain
 * @remark 013F-0143h contains manufacturer code (4 chars) and CGB flag ($80/C0)
 *
 * @see https://gbdev.io/pandocs/The_Cartridge_Header.html
 */
typedef struct RomHeader
{
    u8 entry[4];         // 0100-0103: Entry point (usually nop & jp to 0150)
    u8 logo[0x30];       // 0104-0133: Nintendo logo (must match specific bitmap)
                         // Top half (0104-011B) checked on CGB, full check on DMG

    char title[16];      // 0134-0143: Title in uppercase ASCII (padded with 00s)
                         // Newer carts use 013F-0142 as manufacturer code,
                         // 0143 as CGB flag ($80=enhanced, $C0=CGB only)

    u16 new_lic_code;    // 0144-0145: New licensee code (ASCII, e.g. 00=None, 01=Nintendo)
    u8  sgb_flag;        // 0146: SGB support ($03=enabled, others disable commands)
    u8  type;            // 0147: Cartridge type (MBC1=$01, MBC3=$13, etc.)
    u8  rom_size;        // 0148: ROM size (32KB << value; $00=32KB, $01=64KB, ...)
    u8  ram_size;        // 0149: RAM size ($00=None, $02=8KB, $03=32KB, etc.)
    u8  dest_code;       // 014A: Destination ($00=Japan, $01=Overseas)
    u8  lic_code;        // 014B: Old licensee code ($33 uses new code)
    u8  version;         // 014C: Version number (usually $00)
    u8  checksum;        // 014D: Header checksum (x=0; for 0134-014C: x=x - byte - 1)
    u16 global_checksum; // 014E-014F: ROM checksum (excluding self), not verified by boot ROM
} RomHeader;

// Core functions
void gb_init( GameBoy * gb );
void gb_load_rom( GameBoy * gb, const char * filename );
void gb_step( GameBoy * gb );

void TraceLog( int logLevel, const char * text, ... );

#endif // !GBCE_H
