/****************************** CameCore *********************************
 *
 * Module: IO Operations
 *
 * Handles input/output operations.
 *
 * Key Features:
 * - Memory-mapped IO address management
 *
 * Hardware Registers map:
 * +-----------+------------------+-----------------------------------------------+----------------+-------+
 * | Address   | Register         | Description                                   | Access         | Model |
 * +-----------+------------------+-----------------------------------------------+----------------+-------+
 * | FF00      | P1/JOYP          | Joypad                                        | Mixed          | All   |
 * | FF01      | SB               | Serial transfer data                          | R/W            | All   |
 * | FF02      | SC               | Serial transfer control                       | R/W            | Mixed |
 * | FF04      | DIV              | Divider register                              | R/W            | All   |
 * | FF05      | TIMA             | Timer counter                                 | R/W            | All   |
 * | FF06      | TMA              | Timer modulo                                  | R/W            | All   |
 * | FF07      | TAC              | Timer control                                 | R/W            | All   |
 * | FF0F      | IF               | Interrupt flag                                | R/W            | All   |
 * | FF10      | NR10             | Sound channel 1 sweep                         | R/W            | All   |
 * | FF11      | NR11             | Sound channel 1 length timer & duty cycle     | Mixed          | All   |
 * | FF12      | NR12             | Sound channel 1 volume & envelope             | R/W            | All   |
 * | FF13      | NR13             | Sound channel 1 period low                    | W              | All   |
 * | FF14      | NR14             | Sound channel 1 period high & control         | Mixed          | All   |
 * | FF16      | NR21             | Sound channel 2 length timer & duty cycle     | Mixed          | All   |
 * | FF17      | NR22             | Sound channel 2 volume & envelope             | R/W            | All   |
 * | FF18      | NR23             | Sound channel 2 period low                    | W              | All   |
 * | FF19      | NR24             | Sound channel 2 period high & control         | Mixed          | All   |
 * | FF1A      | NR30             | Sound channel 3 DAC enable                    | R/W            | All   |
 * | FF1B      | NR31             | Sound channel 3 length timer                  | W              | All   |
 * | FF1C      | NR32             | Sound channel 3 output level                  | R/W            | All   |
 * | FF1D      | NR33             | Sound channel 3 period low                    | W              | All   |
 * | FF1E      | NR34             | Sound channel 3 period high & control         | Mixed          | All   |
 * | FF20      | NR41             | Sound channel 4 length timer                  | W              | All   |
 * | FF21      | NR42             | Sound channel 4 volume & envelope             | R/W            | All   |
 * | FF22      | NR43             | Sound channel 4 frequency & randomness        | R/W            | All   |
 * | FF23      | NR44             | Sound channel 4 control                       | Mixed          | All   |
 * | FF24      | NR50             | Master volume & VIN panning                   | R/W            | All   |
 * | FF25      | NR51             | Sound panning                                 | R/W            | All   |
 * | FF26      | NR52             | Sound on/off                                  | Mixed          | All   |
 * | FF30-FF3F | Wave RAM         | Channel 3 waveform storage                    | R/W            | All   |
 * | FF40      | LCDC             | LCD control                                   | R/W            | All   |
 * | FF41      | STAT             | LCD status                                    | Mixed          | All   |
 * | FF42      | SCY              | Viewport Y position                           | R/W            | All   |
 * | FF43      | SCX              | Viewport X position                           | R/W            | All   |
 * | FF44      | LY               | LCD Y coordinate                              | R              | All   |
 * | FF45      | LYC              | LY compare                                    | R/W            | All   |
 * | FF46      | DMA              | OAM DMA source address & start                | R/W            | All   |
 * | FF47      | BGP              | BG palette data                               | R/W            | DMG   |
 * | FF48      | OBP0             | OBJ palette 0 data                            | R/W            | DMG   |
 * | FF49      | OBP1             | OBJ palette 1 data                            | R/W            | DMG   |
 * | FF4A      | WY               | Window Y position                             | R/W            | All   |
 * | FF4B      | WX               | Window X position plus 7                      | R/W            | All   |
 * | FF4D      | KEY1             | Prepare speed switch                          | Mixed          | CGB   |
 * | FF4F      | VBK              | VRAM bank                                     | R/W            | CGB   |
 * | FF51      | HDMA1            | VRAM DMA source high                          | W              | CGB   |
 * | FF52      | HDMA2            | VRAM DMA source low                           | W              | CGB   |
 * | FF53      | HDMA3            | VRAM DMA destination high                     | W              | CGB   |
 * | FF54      | HDMA4            | VRAM DMA destination low                      | W              | CGB   |
 * | FF55      | HDMA5            | VRAM DMA length/mode/start                    | R/W            | CGB   |
 * | FF56      | RP               | Infrared communications port                  | Mixed          | CGB   |
 * | FF68      | BCPS/BGPI        | Background color palette specification        | R/W            | CGB   |
 * | FF69      | BCPD/BGPD        | Background color palette data                 | R/W            | CGB   |
 * | FF6A      | OCPS/OBPI        | OBJ color palette specification               | R/W            | CGB   |
 * | FF6B      | OCPD/OBPD        | OBJ color palette data                        | R/W            | CGB   |
 * | FF6C      | OPRI             | Object priority mode                          | R/W            | CGB   |
 * | FF70      | SVBK             | WRAM bank                                     | R/W            | CGB   |
 * | FF76      | PCM12            | Audio digital outputs 1 & 2                   | R              | CGB   |
 * | FF77      | PCM34            | Audio digital outputs 3 & 4                   | R              | CGB   |
 * | FFFF      | IE               | Interrupt enable                              | R/W            | All   |
 * +-----------+------------------+-----------------------------------------------+----------------+-------+
 * Reference: https://gbdev.io/pandocs/Hardware_Reg_List.html
 *
 *                               LICENSE
 * ------------------------------------------------------------------------
 * Copyright (c) 2025 SOHNE, Leandro Peres (@zschzen)
 *
 * This software is provided "as-is", without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the use
 * of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including
 * commercial applications, and to alter it and redistribute it freely, subject to the
 * following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that
 *      you wrote the original software. If you use this software in a product, an
 *      acknowledgment in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *      as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 *
 *************************************************************************/

#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Module Defines and Macros
//----------------------------------------------------------------------------------------------------------------------
// Joypad
#define JOYPAD_ADDR    0xFF00 /**< Gamepad input register */

// Serial Transfer
#define SB_ADDR        0xFF01 /**< Serial transfer data */
#define SC_ADDR        0xFF02 /**< Serial transfer control */

// Timer
#define DIV_ADDR       0xFF04 /**< Divider register */
#define TIMA_ADDR      0xFF05 /**< Timer counter */
#define TMA_ADDR       0xFF06 /**< Timer modulo */
#define TAC_ADDR       0xFF07 /**< Timer control */

// Interrupts
#define IF_ADDR        0xFF0F /**< Interrupt flag */
#define IE_ADDR        0xFFFF /**< Interrupt enable */

// Sound Channel 1 - Pulse with sweep
#define NR10_ADDR      0xFF10 /**< Sound channel 1 sweep */
#define NR11_ADDR      0xFF11 /**< Sound channel 1 length timer & duty cycle */
#define NR12_ADDR      0xFF12 /**< Sound channel 1 volume & envelope */
#define NR13_ADDR      0xFF13 /**< Sound channel 1 period low */
#define NR14_ADDR      0xFF14 /**< Sound channel 1 period high & control */

// Sound Channel 2 - Pulse
#define NR21_ADDR      0xFF16 /**< Sound channel 2 length timer & duty cycle */
#define NR22_ADDR      0xFF17 /**< Sound channel 2 volume & envelope */
#define NR23_ADDR      0xFF18 /**< Sound channel 2 period low */
#define NR24_ADDR      0xFF19 /**< Sound channel 2 period high & control */

// Sound Channel 3 - Wave
#define NR30_ADDR      0xFF1A /**< Sound channel 3 DAC enable */
#define NR31_ADDR      0xFF1B /**< Sound channel 3 length timer */
#define NR32_ADDR      0xFF1C /**< Sound channel 3 output level */
#define NR33_ADDR      0xFF1D /**< Sound channel 3 period low */
#define NR34_ADDR      0xFF1E /**< Sound channel 3 period high & control */

// Sound Channel 4 - Noise
#define NR41_ADDR      0xFF20 /**< Sound channel 4 length timer */
#define NR42_ADDR      0xFF21 /**< Sound channel 4 volume & envelope */
#define NR43_ADDR      0xFF22 /**< Sound channel 4 frequency & randomness */
#define NR44_ADDR      0xFF23 /**< Sound channel 4 control */

// Sound Control
#define NR50_ADDR      0xFF24 /**< Master volume & VIN panning */
#define NR51_ADDR      0xFF25 /**< Sound panning */
#define NR52_ADDR      0xFF26 /**< Sound on/off */

// Wave Pattern RAM (defined as range)
#define WAVE_RAM_START 0xFF30 /**< Start of wave pattern RAM */
#define WAVE_RAM_END   0xFF3F /**< End of wave pattern RAM */

// LCD Display
#define LCDC_ADDR      0xFF40 /**< LCD control */
#define STAT_ADDR      0xFF41 /**< LCD status */
#define SCY_ADDR       0xFF42 /**< Viewport Y position */
#define SCX_ADDR       0xFF43 /**< Viewport X position */
#define LY_ADDR        0xFF44 /**< LCD Y coordinate */
#define LYC_ADDR       0xFF45 /**< LY compare */
#define DMA_ADDR       0xFF46 /**< OAM DMA source address & start */

// Palettes (DMG only)
#define BGP_ADDR       0xFF47 /**< BG palette data */
#define OBP0_ADDR      0xFF48 /**< OBJ palette 0 data */
#define OBP1_ADDR      0xFF49 /**< OBJ palette 1 data */

// Window Position
#define WY_ADDR        0xFF4A /**< Window Y position */
#define WX_ADDR        0xFF4B /**< Window X position plus 7 */

// CGB Mode Only
#define KEY1_ADDR      0xFF4D /**< Prepare speed switch */
#define VBK_ADDR       0xFF4F /**< VRAM bank */

// CGB DMA
#define HDMA1_ADDR     0xFF51 /**< VRAM DMA source high */
#define HDMA2_ADDR     0xFF52 /**< VRAM DMA source low */
#define HDMA3_ADDR     0xFF53 /**< VRAM DMA destination high */
#define HDMA4_ADDR     0xFF54 /**< VRAM DMA destination low */
#define HDMA5_ADDR     0xFF55 // VRAM DMA length/mode/start

// CGB Infrared
#define RP_ADDR        0xFF56 /**< Infrared communications port */

// CGB Palettes
#define BCPS_ADDR      0xFF68 /**< Background color palette specification / index */
#define BCPD_ADDR      0xFF69 /**< Background color palette data */
#define OCPS_ADDR      0xFF6A /**< OBJ color palette specification / index */
#define OCPD_ADDR      0xFF6B // OBJ color palette data

// CGB Others
#define OPRI_ADDR      0xFF6C /**< Object priority mode */
#define SVBK_ADDR      0xFF70 /**< WRAM bank */
#define PCM12_ADDR     0xFF76 /**< Audio digital outputs 1 & 2 */
#define PCM34_ADDR     0xFF77 /**< Audio digital outputs 3 & 4 */

//----------------------------------------------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Declarations
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Internal Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------------------------------------------
// Read from specified IO address
u8
ReadIO( u16 addr )
{
    if( JOYPAD_ADDR == addr )
        {
            // TODO: Implement gamepad state reading
        }

    LOG( LOG_ERROR, "UNSUPPORTED IO READ %04X", addr );
    return 0;
}

// Write to specified IO address, logs unsupported attempts
void
WriteIO( u16 addr, u8 value )
{
    LOG( LOG_ERROR, "UNSUPPORTED IO WRITE %04X -> %04X", addr, value );
}
