#include "emulator.h"
#include <SDL.h>
#include "camecore/camecore.h"
#include "sdl_window.h"
#ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#endif

static int Running = 1;

// This function implements one iteration of the emulator loop.
static void
EmulatorLoop( void )
{
    SDL_Event Event;
    while( SDL_PollEvent( &Event ) )
        {
            if( Event.type == SDL_QUIT ) Running = 0;
        }

    if( !StepEmulator() ) Running = 0;

    SDLWindowUpdate();

#ifndef __EMSCRIPTEN__
    SDL_Delay( 16 );
#endif
}

void
RunEmulator( void )
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop( EmulatorLoop, 0, 1 );
#else
    while( Running && IsEmulatorRunning() )
        {
            EmulatorLoop();
        }
#endif
}
