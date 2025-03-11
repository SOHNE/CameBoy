#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"

#include "camecore/camecore.h"
#include "emulator.h"
#include "sdl_window.h"

static const char * const Usages[] = {
    "CameBoy [options]",
    NULL,
};

int
main( int argc, char * argv[] )
{
    const char * CartridgePath       = NULL;
    int          Debug               = 0;

    struct argparse_option Options[] = {
        OPT_HELP(),
        OPT_BOOLEAN( 'd', "debug", &Debug, "Enable debug logging", NULL, 0, 0 ),
        OPT_STRING( 'c', "cartridge", &CartridgePath, "Path to the cartridge file", NULL, 0, 0 ),
        OPT_END(),
    };

    struct argparse ArgParse;
    argparse_init( &ArgParse, Options, Usages, 0 );
    argparse_describe( &ArgParse, "\nA Game Boy™ emulator using CameCore and SDL2.",
                       "\nExample: emulator --debug --cartridge /path/to/legal_rom.gb" );
    argc = argparse_parse( &ArgParse, argc, (const char **)argv );

    // Set debug
    SetLogLevel( ( Debug ) ? LOG_DEBUG : LOG_INFO );

    if( !CartridgePath )
        {
            fprintf( stderr,
                     "Error: No cartridge file specified. Use -c or --cartridge to specify the cartridge file.\n" );
            return EXIT_FAILURE;
        }

    // Setup the emulator
    InitEmulator();
    LoadCartridge( (char *)CartridgePath );

    // Initialize window
    if( !InitSDLWindow( "CameBoy Emulator", 480, 432 ) )
        {
            fprintf( stderr, "Failed to initialize SDL window.\n" );
            return EXIT_FAILURE;
        }

    // Run the emulator main loop
    RunEmulator();

    // Cleanup resources
    DestroySDLWindow();

    return EXIT_SUCCESS;
}
