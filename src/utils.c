#include "camecore/utils.h"
#include "camecore/camecore.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int logLevel = LOG_INFO;

//---------------------------------
// Logging Function
//---------------------------------
void
SetLogLevel( int logType )
{
    logLevel = logType;
}

void
TraceLog( int logType, const char * text, ... )
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
