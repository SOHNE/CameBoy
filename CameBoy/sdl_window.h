#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#include <stdbool.h>

// Initializes an window with title and dimensions
bool InitSDLWindow( const char * Title, int Width, int Height );

// Destroys the window and renderer
void DestroySDLWindow( void );

// Updates the windw
void SDLWindowUpdate( void );

#endif // SDLWINDOW_H
