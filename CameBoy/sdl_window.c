#include "sdl_window.h"
#include <SDL2/SDL.h>
#include <stdio.h>

static SDL_Window *   Window   = NULL;
static SDL_Renderer * Renderer = NULL;

bool
InitSDLWindow( const char * Title, int Width, int Height )
{
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
        {
            fprintf( stderr, "SDL_Init Error: %s\n", SDL_GetError() );
            return false;
        }

    Window = SDL_CreateWindow( Title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_SHOWN );
    if( !Window )
        {
            fprintf( stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError() );
            SDL_Quit();
            return false;
        }

    Renderer = SDL_CreateRenderer( Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( !Renderer )
        {
            fprintf( stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError() );
            SDL_DestroyWindow( Window );
            SDL_Quit();
            return false;
        }

    return true;
}

void
DestroySDLWindow( void )
{
    if( Renderer ) SDL_DestroyRenderer( Renderer );
    if( Window ) SDL_DestroyWindow( Window );
    SDL_Quit();
}

void
SDLWindowUpdate( void )
{
    SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 255 );
    SDL_RenderClear( Renderer );

    /** TODO: Frame Rendering */

    SDL_RenderPresent( Renderer );
}
