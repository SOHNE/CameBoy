/****************************** CameCore *********************************
 *
 * Module: Utilities
 *
 * It includes functions for setting log levels, tracing log messages, and safely
 * reading and writing files to/from memory buffers..
 *
 * Key Features:
 * - SetLogLevel: Configures the minimum log level for message display.
 * - TraceLog: Logs messages based on the specified log level.
 * - LoadFileData: Loads a binary file into memory, returning the data and its size.
 * - SaveFileData: Saves binary data to a specified file.
 *
 *                               USAGE
 * ------------------------------------------------------------------------
 * // Set log level to TRACE:
 * SetLogLevel( LOG_TRACE );
 *
 * // Load file data:
 * size_t bytesRead = 0;
 * u8 *fileData = LoadFileData( "example.dat", &bytesRead );
 *
 * // Save file data:
 * bool success = SaveFileData( "output.dat", fileData, bytesRead );
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

#ifndef CAMECORE_UTILS_H
#define CAMECORE_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
// Logging Macros
//----------------------------------------------------------------------------------------------------------------------
#if defined( CC_DEBUG )
#    define LOG( level, ... ) TraceLog( level, __VA_ARGS__ )

#    if defined( SUPPORT_LOG_DEBUG )
#        define LOGD( ... ) TraceLog( LOG_DEBUG, __VA_ARGS__ )
#    else
#        define LOGD( ... ) ( (void)0 )
#    endif
#else
#    define LOG( level, ... ) ( (void)0 )
#    define LOGD( ... )       ( (void)0 )
#endif

//----------------------------------------------------------------------------------------------------------------------
// Assert
//----------------------------------------------------------------------------------------------------------------------
#if defined( __cplusplus )
#    define STATIC_ASSERT( cond, msg ) static_assert( cond, msg )
#else
#    if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 201112L )
#        define STATIC_ASSERT( cond, msg ) _Static_assert( cond, msg )
#    else
#        define STATIC_ASSERT( cond, msg ) typedef char static_assert_##__LINE__[( cond ) ? 1 : -1]
#    endif
#endif

// Runtime assertion
#ifdef CC_DEBUG
#    define ASSERT( cond, msg, ... )                                                                                   \
        do                                                                                                             \
            {                                                                                                          \
                if( !( cond ) )                                                                                        \
                    {                                                                                                  \
                        TraceLog( LOG_FATAL, "%s:%d: Assertion `%s` failed: " msg, __FILE__, __LINE__, #cond,          \
                                  ##__VA_ARGS__ );                                                                     \
                    }                                                                                                  \
            }                                                                                                          \
        while( 0 )
#else
#    define ASSERT( cond, msg, ... ) ( (void)0 )
#endif

#endif // !CAMECORE_UTILS_H
