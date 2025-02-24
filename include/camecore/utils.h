#ifndef CB_UTILS_H
#define CB_UTILS_H

//---------------------------------
// Logging Macros
//---------------------------------
#if defined( CB_DEBUG )
#    define LOG( level, ... ) TraceLog( level, __VA_ARGS__ )

#    if defined( SUPPORT_LOG_DEBUG )
#        define LOGD( ... ) TraceLog( LOG_DEBUG, __VA_ARGS__ )
#    else
#        define LOGD( ... ) (void)0
#    endif
#else
#    define LOG( level, ... ) (void)0
#    define LOGD( ... )       (void)0
#endif

#endif // !CB_UTILS_H
