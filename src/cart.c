#include <stdio.h>
#include <string.h>
#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
// Header Defines
#define HEADER_OFFSET           0x0100 // Starting offset of the header
#define HEADER_CHECKSUM_START   0x0134 // Checksum calculation start offset
#define HEADER_CHECKSUM_END     0x014C // Checksum calculation end offset
#define HEADER_TITLE_STR_LENGTH 15     // Max index for null-terminated title

//----------------------------------------------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------------------------------------------
// Cart state context data
typedef struct PACKED CartContext
{
    // ROM file/data information
    struct
    {
        char        filename[1024]; // Path to ROM file
        size_t      size;           // Size of ROM data
        u8 *        data;           // Raw ROM data
        RomHeader * header;         // Decoded ROM header
    } rom;

} CartContext;

//----------------------------------------------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// Cart state context
CartContext ctx = { 0 };

// Kind of hardware is present on the cartridge
static const char * ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

// ASCII “licensee code” indicating the game’s publisher
static const char * LIC_CODE[0xA5] = { [0x00] = "None",
                                       [0x01] = "Nintendo R&D1",
                                       [0x08] = "Capcom",
                                       [0x13] = "Electronic Arts",
                                       [0x18] = "Hudson Soft",
                                       [0x19] = "b-ai",
                                       [0x20] = "kss",
                                       [0x22] = "pow",
                                       [0x24] = "PCM Complete",
                                       [0x25] = "san-x",
                                       [0x28] = "Kemco Japan",
                                       [0x29] = "seta",
                                       [0x30] = "Viacom",
                                       [0x31] = "Nintendo",
                                       [0x32] = "Bandai",
                                       [0x33] = "Ocean/Acclaim",
                                       [0x34] = "Konami",
                                       [0x35] = "Hector",
                                       [0x37] = "Taito",
                                       [0x38] = "Hudson",
                                       [0x39] = "Banpresto",
                                       [0x41] = "Ubi Soft",
                                       [0x42] = "Atlus",
                                       [0x44] = "Malibu",
                                       [0x46] = "angel",
                                       [0x47] = "Bullet-Proof",
                                       [0x49] = "irem",
                                       [0x50] = "Absolute",
                                       [0x51] = "Acclaim",
                                       [0x52] = "Activision",
                                       [0x53] = "American sammy",
                                       [0x54] = "Konami",
                                       [0x55] = "Hi tech entertainment",
                                       [0x56] = "LJN",
                                       [0x57] = "Matchbox",
                                       [0x58] = "Mattel",
                                       [0x59] = "Milton Bradley",
                                       [0x60] = "Titus",
                                       [0x61] = "Virgin",
                                       [0x64] = "LucasArts",
                                       [0x67] = "Ocean",
                                       [0x69] = "Electronic Arts",
                                       [0x70] = "Infogrames",
                                       [0x71] = "Interplay",
                                       [0x72] = "Broderbund",
                                       [0x73] = "sculptured",
                                       [0x75] = "sci",
                                       [0x78] = "THQ",
                                       [0x79] = "Accolade",
                                       [0x80] = "misawa",
                                       [0x83] = "lozc",
                                       [0x86] = "Tokuma Shoten Intermedia",
                                       [0x87] = "Tsukuda Original",
                                       [0x91] = "Chunsoft",
                                       [0x92] = "Video system",
                                       [0x93] = "Ocean/Acclaim",
                                       [0x95] = "Varie",
                                       [0x96] = "Yonezawa/s’pal",
                                       [0x97] = "Kaneko",
                                       [0x99] = "Pack in soft",
                                       [0xA4] = "Konami (Yu-Gi-Oh!)" };

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Getters
//----------------------------------------------------------------------------------------------------------------------
// Get cart game’s publisher based on `ROM_TYPES` lookup table
const char *
GetCartTypeName( void )
{
    if( ctx.rom.header->type <= 0x22 )
        {
            return ROM_TYPES[ctx.rom.header->type];
        }

    return "UNKNOWN";
}

// Get the hardware that is present on the cartridge based on `LIC_CODE` lookup table
const char *
GetCartLicenseeName( void )
{
    if( ctx.rom.header->new_lic_code <= 0xA4 )
        {
            return LIC_CODE[ctx.rom.header->lic_code];
        }

    return "UNKNOWN";
}

// Get the calculated header check sum
// NOTE: If the byte at $014D does not match the lower 8 bits of checksum,
// the boot ROM will lock up and the program in the cartridge won’t run.
unsigned char
GetHeaderChecksum( void * romData )
{
    unsigned short  checksumCalc = 0;
    unsigned char * romBytes     = (unsigned char *)romData;

    for( size_t i = HEADER_CHECKSUM_START; i <= HEADER_CHECKSUM_END; i++ )
        {
            checksumCalc = checksumCalc - romBytes[i] - 1;
        }

    return (unsigned char)( checksumCalc & 0xFF );
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: File
//----------------------------------------------------------------------------------------------------------------------
// Loads the cartridge into `CartContext`
bool
CartLoad( char * cartPath )
{
    if( false == IS_STR_VALID( cartPath ) )
        {
            LOG( LOG_ERROR, "Invalid cartridge filename." );
            return false;
        }

    // Init context and copy filename
    memset( &ctx, 0, sizeof( ctx ) );
    snprintf( ctx.rom.filename, sizeof( ctx.rom.filename ), "%s", cartPath );

    // Open cart file
    FILE * const cartFile = fopen( cartPath, "rb" );
    if( NULL == cartFile )
        {
            LOG( LOG_INFO, "Failed to open: %s", cartPath );
            return false;
        }

    // Get file size
    if( 0 != fseek( cartFile, 0, SEEK_END ) )
        {
            LOG( LOG_ERROR, "Unable to read cartridge file: %s", cartPath );
            fclose( cartFile );
            return false;
        }

    ctx.rom.size = ftell( cartFile );

    // Validate file size
    if( (long)( HEADER_OFFSET + sizeof( RomHeader ) ) > ctx.rom.size )
        {
            LOG( LOG_ERROR, "File too small for valid header: %s", cartPath );
            fclose( cartFile );
            return false;
        }

    rewind( cartFile );

    // Alloc memory
    ctx.rom.data = malloc( ctx.rom.size );
    if( NULL == ctx.rom.data )
        {
            LOG( LOG_ERROR, "Memory allocation failed for ROM data." );
            fclose( cartFile );
            return false;
        }

    // Read data
    size_t readBytes = fread( ctx.rom.data, 1, ctx.rom.size, cartFile );
    fclose( cartFile );
    if( readBytes != ctx.rom.size )
        {
            LOG( LOG_ERROR, "Incomplete ROM data read from: %s", cartPath );
            free( ctx.rom.data );
            return false;
        }

    // Setup header and null-terminate title
    ctx.rom.header                                 = (RomHeader *)( (unsigned char *)ctx.rom.data + HEADER_OFFSET );
    ctx.rom.header->title[HEADER_TITLE_STR_LENGTH] = '\0';

    // Verify checksum
    const unsigned char calcChksum = GetHeaderChecksum( ctx.rom.data );
    const bool          chkValid   = ( ctx.rom.header->checksum == calcChksum );

    // Log cart info
    LOG( LOG_INFO, "Cartridge Loaded:" );
    LOG( LOG_INFO, "    Title    : %s", ctx.rom.header->title );
    LOG( LOG_INFO, "    Type     : %02X (%s)", ctx.rom.header->type, GetCartTypeName() );
    LOG( LOG_INFO, "    ROM Size : %zu KB", 32UL << ctx.rom.header->rom_size );
    LOG( LOG_INFO, "    RAM Size : %02X", ctx.rom.header->ram_size );
    LOG( LOG_INFO, "    LIC Code : %02X (%s)", ctx.rom.header->lic_code, GetCartLicenseeName() );
    LOG( LOG_INFO, "    ROM Vers : %02X", ctx.rom.header->version );
    LOG( LOG_INFO, "    Checksum : %02X (%s)", ctx.rom.header->checksum, ( chkValid ) ? "PASSED" : "FAILED" );

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Operations
//----------------------------------------------------------------------------------------------------------------------
// Perform read operation onto cartridge
u8
CartRead( u16 address )
{
    UNUSED( address );
    NO_IMPL();
}

// Perform write operation onto cartridge
void
CartWrite( u16 address, u8 value )
{
    UNUSED( address );
    UNUSED( value );
    NO_IMPL();
}
