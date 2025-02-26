#ifndef CB_UTILS_H
#define CB_UTILS_H

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
#    define ASSERT( cond )                                                                                             \
        do                                                                                                             \
            {                                                                                                          \
                if( !( cond ) )                                                                                        \
                    {                                                                                                  \
                        TraceLog( LOG_FATAL, "%s:%d: Assertion `%s` failed", __FILE__, __LINE__, #cond );              \
                    }                                                                                                  \
            }                                                                                                          \
        while( 0 )
#else
#    define ASSERT( cond ) ( (void)0 )
#endif

#endif // !CB_UTILS_H
