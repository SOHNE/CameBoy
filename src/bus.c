/**
 *  MEMORY MAP
 *  16-bit address bus
 *
 * +-----------+--------------------------------------+----------------------------------------------+
 * | Range     | Description                          | Notes                                        |
 * +-----------+--------------------------------------+----------------------------------------------+
 * | 0000-3FFF | ROM Bank 0 (16 KiB)                  | Fixed bank from cartridge                    |
 * | 4000-7FFF | ROM Bank 1 (16 KiB)                  | Switchable via mapper (if available)         |
 * | 8000-9FFF | Video RAM (8 KiB total)              | In CGB mode, VRAM bank 0/1 may be switchable |
 * |           |   8000-97FF: CHR RAM                 |                                              |
 * |           |   9800-9BFF: BG Map 1                |                                              |
 * |           |   9C00-9FFF: BG Map 2                |                                              |
 * | A000-BFFF | Cartridge (External) RAM (8 KiB)     | Switchable                                   |
 * | C000-CFFF | Work RAM Bank 0 (WRAM, 4 KiB)        |                                              |
 * | D000-DFFF | Work RAM Bank 1-7 (WRAM, 4 KiB each) | Switchable (Color only / CGB mode)           |
 * | E000-FDFF | Echo RAM                             | Mirror of C000-DDFF; Do not use              |
 * | FE00-FE9F | Object Attribute Memory (OAM)        |                                              |
 * | FEA0-FEFF | Unusable                             | Reserved/Prohibited by Nintendo              |
 * | FF00-FF7F | I/O Registers                        |                                              |
 * | FF80-FFFE | High RAM (HRAM / Zero Page)          |                                              |
 * | FFFF-FFFF | Interrupt Enable Register (IE)       |                                              |
 * +-----------+--------------------------------------+----------------------------------------------+
 * REFERENCE: https://gbdev.io/pandocs/Memory_Map.html
*/

#include "camecore/camecore.h"
#include "camecore/utils.h"

//----------------------------------------------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------------------------------------------
#define ROM_BANK_SIZE      0x4000 /**< Size of each ROM bank (16 KiB) */
#define ROM_BANK0_START    0x0000 /**< Start address of fixed ROM bank 0 */
#define ROM_BANK0_END      0x3FFF /**< End address of fixed ROM bank 0 */
#define ROM_BANKN_START    0x4000 /**< Start address of switchable ROM bank */
#define ROM_BANKN_END      0x7FFF /**< End address of switchable ROM bank */

#define VRAM_SIZE          0x2000 /**< Total size of Video RAM (8 KiB) */
#define VRAM_START         0x8000 /**< Start address of VRAM */
#define VRAM_END           0x9FFF /**< End address of VRAM */
#define VRAM_CHR_RAM_START 0x8000 /**< Start address of CHR RAM */
#define VRAM_CHR_RAM_END   0x97FF /**< End address of CHR RAM */
#define VRAM_BG_MAP1_START 0x9800 /**< Start address of Background Map 1 */
#define VRAM_BG_MAP1_END   0x9BFF /**< End address of Background Map 1 */
#define VRAM_BG_MAP2_START 0x9C00 /**< Start address of Background Map 2 */
#define VRAM_BG_MAP2_END   0x9FFF /**< End address of Background Map 2 */

#define EXTRAM_SIZE        0x2000 /**< Size of external cartridge RAM (8 KiB) */
#define EXTRAM_START       0xA000 /**< Start address of external RAM */
#define EXTRAM_END         0xBFFF /**< End address of external RAM */

#define WRAM_SIZE          0x2000 /**< Total size of Work RAM (8 KiB) */
#define WRAM_START         0xC000 /**< Start address of Work RAM */
#define WRAM_END           0xDFFF /**< End address of Work RAM */
#define WRAM_BANKN_START   0xD000 /**< Start address of switchable WRAM banks */
#define WRAM_BANKN_END     0xDFFF /**< End address of switchable WRAM banks */

#define ECHO_SIZE          0x1E00 /**< Size of Echo RAM */
#define ECHO_START         0xE000 /**< Start address of Echo RAM (mirrors C000-DDFF) */
#define ECHO_END           0xFDFF /**< End address of Echo RAM */

#define OAM_SIZE           0xA0   /**< Size of Object Attribute Memory (OAM) */
#define OAM_START          0xFE00 /**< Start address of OAM */
#define OAM_END            0xFE9F /**< End address of OAM */

#define IO_SIZE            0x80   /**< Size of I/O register space */
#define IO_START           0xFF00 /**< Start address of I/O registers */
#define IO_END             0xFF7F /**< End address of I/O registers */

#define HRAM_SIZE          0x7F   /**< Size of High RAM (HRAM) */
#define HRAM_START         0xFF80 /**< Start address of High RAM */
#define HRAM_END           0xFFFE /**< End address of High RAM */

#define IE_REGISTER        0xFFFF /**< Address of the Interrupt Enable Register */

//----------------------------------------------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------------------------------------------
u8
BusRead( u16 addr )
{
    // Cartridge ROM: 0x0000–0x7FFF
    if( BETWEEN( addr, ROM_BANK0_START, ROM_BANKN_END ) )
        {
            return CartRead( addr );
        }

    NO_IMPL();
    return 0;
}

void
BusWrite( u16 addr, u8 value )
{
    // Cartridge ROM: 0x0000–0x7FFF
    if( BETWEEN( addr, ROM_BANK0_START, ROM_BANKN_END ) )
        {
            CartWrite( addr, value );
            return;
        }

    NO_IMPL();
}
