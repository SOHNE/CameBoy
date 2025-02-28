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

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Utilities
//----------------------------------------------------------------------------------------------------------------------
void
SetLogLevel( i32 logType )
{
    logLevel = logType;
}

void
TraceLog( i32 logType, const char * text, ... )
{
    if( logType < logLevel ) return;

    char levelStr[8];

    // Determine the log level string safely
    switch( logType )
        {
            case LOG_TRACE:   strcpy( levelStr, "TRACE" ); break;
            case LOG_DEBUG:   strcpy( levelStr, "DEBUG" ); break;
            case LOG_INFO:    strcpy( levelStr, "INFO" ); break;
            case LOG_WARNING: strcpy( levelStr, "WARNING" ); break;
            case LOG_ERROR:   strcpy( levelStr, "ERROR" ); break;
            case LOG_FATAL:   strcpy( levelStr, "FATAL" ); break;
            default:          strcpy( levelStr, "UNKNOWN" ); break;
        }

    fprintf( stderr, "[%s] ", levelStr );

    va_list args;
    va_start( args, text );
    vfprintf( stderr, text, args );
    va_end( args );

    fprintf( stderr, "\n" );

    if( logType == LOG_FATAL ) abort();
}

u8 *
LoadFileData( const char * filename, size_t * bytesRead )
{
    *bytesRead = 0;

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

    *bytesRead = fread( buffer, 1, size, file );
    if( size != *bytesRead )
        {
            LOG( LOG_ERROR, "FILEIO: [%s] File partially read (%zu of %u bytes)", filename, *bytesRead, size );
            free( buffer );
            fclose( file );
            *bytesRead = 0;
            return NULL;
        }

    fclose( file );
    LOG( LOG_INFO, "FILEIO: [%s] File loaded successfully (%zu bytes)", filename, *bytesRead );
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
