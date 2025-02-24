#ifndef CAMECORE_CCAPI_H
#define CAMECORE_CCAPI_H

#include "camecore/export.h"

#if defined( CC_STATIC_DEFINE )
#    define CCAPI
#else
#    if defined( _WIN32 ) || defined( __CYGWIN__ )
#        ifdef CameCore_EXPORTS
#            ifdef __GNUC__
#                define CCAPI __attribute__( ( dllexport ) )
#            else
#                define CCAPI __declspec( dllexport )
#            endif
#        else
#            ifdef __GNUC__
#                define CCAPI __attribute__( ( dllimport ) )
#            else
#                define CCAPI __declspec( dllimport )
#            endif
#        endif
#    else
#        if __GNUC__ >= 4
#            define CCAPI __attribute__( ( visibility( "default" ) ) )
#        else
#            define CCAPI
#        endif
#    endif
#endif

#endif // !CAMECORE_CC_API_H
