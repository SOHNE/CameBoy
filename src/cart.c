/****************************** CameCore *********************************
 *
 * Module: cartridge
 *
 * It defines the ROM header structure, cartridge context, and operations
 * for loading, validating, and accessing cartridge data.
 *
 * Key Features:
 * - Parsing and validating ROM header data
 * - Loading cartridge data from file
 * - Providing utility functions for cartridge type and licensee lookup
 * - Exposing functions for read and write operations on cartridge memory
 *
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *      wrote the original software. If you use this software in a product, an acknowledgment
 *      in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *      as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 *
 *************************************************************************/

#include "camecore/camecore.h"
#include "camecore/utils.h"
#include <stdio.h>
#include <string.h>

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
/**
 * @struct RomHeader
 * @brief Game Boy cartridge header (0100-014Fh range)
 *
 * Defines ROM metadata including boot behavior, hardware requirements, and validation data.
 * Critical fields:
 * - Entry point instructions
 * - Nintendo logo bitmap (verified at boot)
 * - Title/manufacturer codes
 * - CGB/SGB compatibility flags
 * - Memory configuration (MBC type, ROM/RAM sizes)
 * - Checksums and regional codes
 *
 * @note Logo bytes (0104-0133h) must match Nintendo's bitmap or boot fails
 * @warning Header checksum (014Dh) must validate via 0134h-014Ch subtraction chain
 * @remark 013F-0143h contains manufacturer code (4 chars) and CGB flag ($80/C0)
 *
 * @see https://gbdev.io/pandocs/The_Cartridge_Header.html
 */
typedef struct PACKED RomHeader
{
    u8 entry[4];         /**< 0100-0103: Entry point (usually nop & jp to 0150) */
    u8 logo[0x30];       /**< 0104-0133: Nintendo logo (must match specific bitmap)
                              Top half (0104-011B) checked on CGB, full check on DMG */

    char title[16];      /**< 0134-0143: Title in uppercase ASCII (padded with 00s)
                              Newer carts use 013F-0142 as manufacturer code,
                              0143 as CGB flag ($80=enhanced, $C0=CGB only) */

    u16 new_lic_code;    /**< 0144-0145: New licensee code (ASCII, e.g. 00=None, 01=Nintendo) */
    u8  sgb_flag;        /**< 0146: SGB support ($03=enabled, others disable commands) */
    u8  type;            /**< 0147: Cartridge type (MBC1=$01, MBC3=$13, etc.) */
    u8  rom_size;        /**< 0148: ROM size (32KB << value; $00=32KB, $01=64KB, ...) */
    u8  ram_size;        /**< 0149: RAM size ($00=None, $02=8KB, $03=32KB, etc.) */
    u8  dest_code;       /**< 014A: Destination ($00=Japan, $01=Overseas) */
    u8  lic_code;        /**< 014B: Old licensee code ($33 uses new code) */
    u8  version;         /**< 014C: Version number (usually $00) */
    u8  checksum;        /**< 014D: Header checksum (x=0; for 0134-014C: x=x - byte - 1) */
    u16 global_checksum; /**< 014E-014F: ROM checksum (excluding self), not verified by boot ROM */
} RomHeader;

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
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// Cart state context
CartContext cart_ctx            = { 0 };

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
static const char * LIC_CODE[0xA5] = {
    [0x00] = "None",
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
    [0xA4] = "Konami (Yu-Gi-Oh!)",
};

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Getters
//----------------------------------------------------------------------------------------------------------------------
// Get the hardware that is present on the cartridge based on `LIC_CODE` lookup table
static const char *
GetCartTypeName( void )
{
    if( 0x22 >= cart_ctx.rom.header->type )
        {
            return ROM_TYPES[cart_ctx.rom.header->type];
        }
    return "UNKNOWN";
}

// Get cart game’s publisher based on `ROM_TYPES` lookup table
static const char *
GetCartLicenseeName( void )
{
    if( 0xA4 >= cart_ctx.rom.header->new_lic_code )
        {
            return LIC_CODE[cart_ctx.rom.header->lic_code];
        }
    return "UNKNOWN";
}

// Get the calculated header check sum
// NOTE: If the byte at $014D does not match the lower 8 bits of checksum,
// the boot ROM will lock up and the program in the cartridge won’t run.
static u16
GetHeaderChecksum( void )
{
    u16 checksumCalc = 0;

    for( size_t i = HEADER_CHECKSUM_START; i <= HEADER_CHECKSUM_END; ++i )
        {
            checksumCalc = checksumCalc - cart_ctx.rom.data[i] - 1;
        }

    return LOW_BYTE( checksumCalc );
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: File
//----------------------------------------------------------------------------------------------------------------------
// Loads the cartridge into `CartContext`
bool
LoadCartridge( char * cartPath )
{
    size_t bytesRead;
    u8 *   fileData;
    bool   chkValid;

    if( false == IS_STR_VALID( cartPath ) )
        {
            LOG( LOG_ERROR, "Invalid cartridge filename." );
            return false;
        }

    // Init context
    memset( &cart_ctx, 0, sizeof( cart_ctx ) );
    snprintf( cart_ctx.rom.filename, sizeof( cart_ctx.rom.filename ), "%s", cartPath );

    // Load cartrige file
    fileData = LoadFileData( cartPath, &bytesRead );
    if( NULL == fileData || 0 == bytesRead ) return false;

    // Validate file size
    if( ( HEADER_OFFSET + sizeof( RomHeader ) ) > bytesRead )
        {
            LOG( LOG_ERROR, "File too small for valid header: %s", cartPath );
            free( fileData );
            return false;
        }

    cart_ctx.rom.size   = bytesRead;
    cart_ctx.rom.data   = fileData;

    // Setup header and null-terminate title
    cart_ctx.rom.header = (RomHeader *)( cart_ctx.rom.data + HEADER_OFFSET );
    cart_ctx.rom.header->title[sizeof( cart_ctx.rom.header->title ) - 1] = '\0';

    // Verify checksum
    chkValid = ( GetHeaderChecksum() == cart_ctx.rom.header->checksum );

    // Log cart info
    LOG( LOG_INFO, "Cartridge Loaded:" );
    LOG( LOG_INFO, "    > Title    : %s", cart_ctx.rom.header->title );
    LOG( LOG_INFO, "    > Type     : %02X (%s)", cart_ctx.rom.header->type, GetCartTypeName() );
    LOG( LOG_INFO, "    > ROM Size : %zu KB", (size_t)( 32UL << cart_ctx.rom.header->rom_size ) );
    LOG( LOG_INFO, "    > RAM Size : %02X", cart_ctx.rom.header->ram_size );
    LOG( LOG_INFO, "    > LIC Code : %02X (%s)", cart_ctx.rom.header->lic_code, GetCartLicenseeName() );
    LOG( LOG_INFO, "    > ROM Vers : %02X", cart_ctx.rom.header->version );
    LOG( LOG_INFO, "    > Checksum : %02X (%s)", cart_ctx.rom.header->checksum, ( chkValid ) ? "PASSED" : "FAILED" );

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition: Operations
//----------------------------------------------------------------------------------------------------------------------
// Perform read operation on cartridge
u8
ReadCartridge( u16 address )
{
    return cart_ctx.rom.data[address];
}

// Perform write operation on cartridge
void
WriteCartridge( u16 address, u8 value )
{
    UNUSED( address );
    UNUSED( value );
    NO_IMPL();
}
