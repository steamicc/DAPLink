/**
 * @file    steami_config.h
 * @brief   STeaMi board config zone in internal flash (F103 gap area)
 *
 * The config zone occupies the 1 KB gap between the bootloader and the
 * interface firmware (0x0800BC00 - 0x0800BFFF).  This area survives
 * interface firmware updates, making it suitable for factory-programmed
 * data such as board revision, name, and sensor calibration.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "daplink_addr.h"
#include "compiler.h"

/** Start address of the config zone (BL/IF gap). */
#define STEAMI_CONFIG_ADDR  (DAPLINK_ROM_BL_START + DAPLINK_ROM_BL_SIZE)

/** Size of the config zone in bytes (one flash page). */
#define STEAMI_CONFIG_SIZE  DAPLINK_SECTOR_SIZE

/* Compile-time check: config zone must fit between bootloader and interface. */
COMPILER_ASSERT(STEAMI_CONFIG_ADDR + STEAMI_CONFIG_SIZE <= DAPLINK_ROM_IF_START);

/**
 * @brief Read data from the config zone.
 *
 * The config zone is memory-mapped, so this is a simple memcpy.
 *
 * @param offset  Byte offset within the config zone (0 .. STEAMI_CONFIG_SIZE-1)
 * @param buf     Destination buffer
 * @param len     Number of bytes to read
 * @return true on success, false if offset+len exceeds the zone
 */
bool steami_config_read(uint16_t offset, uint8_t *buf, uint16_t len);

/**
 * @brief Write data to the config zone.
 *
 * Erases the entire 1 KB page then programs the supplied data.
 * A shadow buffer is used to preserve existing content outside
 * the written range.
 *
 * @param offset  Byte offset within the config zone
 * @param data    Source data
 * @param len     Number of bytes to write
 * @return true on success, false on parameter error or flash failure
 */
bool steami_config_write(uint16_t offset, const uint8_t *data, uint16_t len);

/**
 * @brief Erase the entire config zone (fill with 0xFF).
 *
 * @return true on success, false on flash erase failure
 */
bool steami_config_erase(void);
