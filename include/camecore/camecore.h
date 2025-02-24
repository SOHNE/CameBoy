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
#define UNUSED( x ) CLITERAL( (void)( x ) )

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
// Assert
//---------------------------------
#if defined( __cplusplus )
#    define CC_STATIC_ASSERT( cond, msg ) static_assert( cond, msg )
#else
#    if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 201112L )
#        define CC_STATIC_ASSERT( cond, msg ) _Static_assert( cond, msg )
#    else
// Force a division-by-zero error with a message
#        define CC_STATIC_ASSERT( cond, msg )                                                                          \
            enum                                                                                                       \
            {                                                                                                          \
                CC_STATIC_ASSERT_##__LINE__ = 1 / ( !!( cond ) )                                                       \
            }
#    endif
#endif

#define CC_ASSERT( cond, msg )                                                                                         \
    if( !( cond ) ) panic( msg )

//---------------------------------
// Bit operations
//---------------------------------
#define BIT( n )                  ( 0x1U << ( n ) )
#define BIT_SET( v, n )           ( ( v ) |= BIT( n ) )
#define BIT_CLEAR( v, n )         ( ( v ) &= ~BIT( n ) )
#define BIT_TEST( v, n )          ( !!( ( v ) & BIT( n ) ) )
#define BIT_TOGGLE( v, n )        ( ( v ) ^= BIT( n ) )     // Toggle bit
#define BIT_MASK( len )           ( ( 1U << ( len ) ) - 1 ) // Create a bit mask of 'len' bits
// 8-bit register manipulation (common in CC hardware)
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

//---------------------------------
// Error handling
//---------------------------------
NORETURN static INLINE void
panic( const char * msg )
{
    exit( EXIT_FAILURE );
}

// Core functions
void gb_init( GameBoy * gb );
void gb_load_rom( GameBoy * gb, const char * filename );
void gb_step( GameBoy * gb );

void TraceLog( int logLevel, const char * text, ... );

#endif // !GBCE_H
