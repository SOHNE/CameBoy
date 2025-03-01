#ifndef GBCE_H
#define GBCE_H

#include "camecore/version.h"
#include "ccapi.h"

#include <stdint.h>
#include <stdlib.h>

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
#define IS_STR_VALID( str ) ( ( str ) != NULL && ( str )[0] != '\0' )

// Direct bit operations
#define BIT( n )            ( 0x01U << ( n ) )
#define BIT_SET( r, n )     ( ( r ) |= BIT( n ) )
#define BIT_CLEAR( r, n )   ( ( r ) &= ~BIT( n ) )
#define BIT_TOGGLE( r, n )  ( ( r ) ^= BIT( n ) )
#define BIT_CHECK( r, n )   ( ( r ) & BIT( n ) )

// Flags operation macros
#define FLAG_SET( n, f )    ( ( n ) |= ( f ) )
#define FLAG_CLEAR( n, f )  ( ( n ) &= ~( f ) )
#define FLAG_TOGGLE( n, f ) ( ( n ) ^= ( f ) )
#define FLAG_CHECK( n, f )  ( ( n ) & ( f ) )

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

//----------------------------------------------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------------------------------------------
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
// Functions Declaration
//----------------------------------------------------------------------------------------------------------------------

CXX_GUARD_START

// Core
//------------------------------------------------------------------
CCAPI void Init( void );

// Bus
//------------------------------------------------------------------
CCAPI u8   BusRead( u16 addr );
CCAPI void BusWrite( u16 addr, u8 value );

// Cart
//------------------------------------------------------------------
CCAPI bool CartLoad( char * cart );
CCAPI u8   CartRead( u16 address );
CCAPI void CartWrite( u16 address, u8 value );

// Utils
//------------------------------------------------------------------
CCAPI void TraceLog( i32 logLevel, const char * text, ... );
CCAPI void SetLogLevel( i32 logLevel );

CCAPI u8 * LoadFileData( const char * filename, size_t * outBytesRead );
CCAPI bool SaveFileData( const char * filename, const u8 * data, size_t dataSize );

CXX_GUARD_END

#endif // !GBCE_H
