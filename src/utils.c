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

#include "camecore/utils.h"
#include "camecore/camecore.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------
#ifndef MAX_FILEPATH_LENGTH
#    if defined( _WIN32 )
#        define MAX_FILEPATH_LENGTH 260
#    else
#        define MAX_FILEPATH_LENGTH 4096
#    endif
#endif

//----------------------------------------------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------------------------------------------
static int logLevel = LOG_INFO;

// Callbacks
static TraceLogCallback traceLog = NULL;

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Utilities
//----------------------------------------------------------------------------------------------------------------------
void
SetTraceLogCallback( TraceLogCallback callback )
{
    traceLog = callback;
}

void
SetLogLevel( i32 logType )
{
    logLevel = logType;
}

void
TraceLog( i32 logType, const char * text, ... )
{
    va_list      args;
    const char * levelString;

    // Skip logging
    if( logType < logLevel ) return;

    va_start( args, text );

    // Use custom callback if available
    if( NULL != traceLog )
        {
            traceLog( logType, text, args );
            va_end( args );
            return;
        }

    // Default logging behavior - determine log level string
    switch( logType )
        {
            case LOG_TRACE:   levelString = "TRACE"; break;
            case LOG_DEBUG:   levelString = "DEBUG"; break;
            case LOG_INFO:    levelString = "INFO"; break;
            case LOG_WARNING: levelString = "WARNING"; break;
            case LOG_ERROR:   levelString = "ERROR"; break;
            case LOG_FATAL:   levelString = "FATAL"; break;
            default:          levelString = "UNKNOWN"; break;
        }

    // Print log message
    fprintf( stderr, "[%s] ", levelString );
    vfprintf( stderr, text, args );
    fprintf( stderr, "\n" );

    va_end( args );

    // Handle fatal errors
    if( logType == LOG_FATAL ) abort();
}

u8 *
LoadFileData( const char * filename, size_t * outBytesRead )
{
    *outBytesRead = 0;

    // Validate filename
    if( !IS_STR_VALID( filename ) || strlen( filename ) > MAX_FILEPATH_LENGTH )
        {
            LOG( LOG_ERROR, "FILEIO: Invalid filename provided" );
            return NULL;
        }

    FILE * const file = fopen( filename, "rb" );
    if( !file )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] Failed to open file", filename );
            return NULL;
        }

    // Determine file size
    fseek( file, 0, SEEK_END );
    size_t size = ftell( file );
    fseek( file, 0, SEEK_SET );

    if( 0 == size )
        {
            LOG( LOG_WARNING, "FILEIO: [%s] Empty file", filename );
            fclose( file );
            return NULL;
        }

    // Allocate buffer
    u8 * const buffer = (u8 *)malloc( size );
    if( !buffer )
        {
            LOG( LOG_ERROR, "FILEIO: Failed to allocate %u bytes for file %s", size, filename );
            fclose( file );
            return NULL;
        }

    *outBytesRead = fread( buffer, 1, size, file );
    if( size != *outBytesRead )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] File partially read (%zu of %u bytes)", filename, *outBytesRead, size );
            free( buffer );
            fclose( file );
            *outBytesRead = 0;
            return NULL;
        }

    fclose( file );
    LOG( LOG_INFO, "FILEIO: [%s] File loaded successfully (%zu bytes)", filename, *outBytesRead );
    return buffer;
}

bool
SaveFileData( const char * filename, const u8 * data, size_t dataSize )
{
    // Validate
    if( !IS_STR_VALID( filename ) || strlen( filename ) > MAX_FILEPATH_LENGTH )
        {
            LOG( LOG_ERROR, "FILEIO: Invalid filename provided" );
            return false;
        }
    // Validate that data is not NULL
    if( !data )
        {
            LOG( LOG_ERROR, "FILEIO: Invalid data buffer provided" );
            return false;
        }
    // If dataSize is zero, log a warning and skip writing
    if( 0 == dataSize )
        {
            LOG( LOG_WARNING, "FILEIO: Skipping write of empty buffer to file %s", filename );
            return false;
        }

    // Open the file in binary write mode
    FILE * const file = fopen( filename, "wb" );
    if( !file )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] Failed to open file for writing", filename );
            return false;
        }

    // Write the provided data to the file
    const size_t written = fwrite( data, 1, dataSize, file );
    if( dataSize != written )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] File partially written (%zu of %zu bytes)", filename, written, dataSize );
            fclose( file );
            return false;
        }

    // Close the file and check for any errors
    if( 0 != fclose( file ) )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] Failed to close file", filename );
            return false;
        }

    LOG( LOG_INFO, "FILEIO: [%s] File saved successfully (%zu bytes)", filename, dataSize );
    return true;
}
