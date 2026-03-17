/**
 * @file    steami_config.c
 * @brief   STeaMi board config zone in internal flash (F103 gap area)
 */

#include "steami_config.h"
#include "stm32f1xx.h"
#include <string.h>

/** Shadow buffer used during read-modify-write cycles. */
static uint8_t shadow[STEAMI_CONFIG_SIZE] __attribute__((aligned(4)));

bool steami_config_read(uint16_t offset, uint8_t *buf, uint16_t len)
{
    if ((uint32_t)offset + len > STEAMI_CONFIG_SIZE) {
        return false;
    }

    /* Config zone is memory-mapped — direct read. */
    memcpy(buf, (const uint8_t *)(STEAMI_CONFIG_ADDR + offset), len);
    return true;
}

bool steami_config_write(uint16_t offset, const uint8_t *data, uint16_t len)
{
    if ((uint32_t)offset + len > STEAMI_CONFIG_SIZE) {
        return false;
    }

    /* 1. Read current page into shadow buffer. */
    memcpy(shadow, (const uint8_t *)STEAMI_CONFIG_ADDR, STEAMI_CONFIG_SIZE);

    /* 2. Merge new data into shadow. */
    memcpy(shadow + offset, data, len);

    /* 3. Erase the page. */
    FLASH_EraseInitTypeDef erase;
    uint32_t error;

    HAL_FLASH_Unlock();

    memset(&erase, 0, sizeof(erase));
    erase.TypeErase  = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = STEAMI_CONFIG_ADDR;
    erase.NbPages    = 1;

    if (HAL_FLASHEx_Erase(&erase, &error) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    /* 4. Program shadow buffer back (word-by-word). */
    for (uint32_t i = 0; i < STEAMI_CONFIG_SIZE; i += 4) {
        uint32_t word;
        memcpy(&word, shadow + i, 4);

        /* Skip words that are already erased (0xFFFFFFFF). */
        if (word == 0xFFFFFFFF) {
            continue;
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                              STEAMI_CONFIG_ADDR + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    HAL_FLASH_Lock();
    return true;
}

bool steami_config_erase(void)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t error;

    HAL_FLASH_Unlock();

    memset(&erase, 0, sizeof(erase));
    erase.TypeErase  = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = STEAMI_CONFIG_ADDR;
    erase.NbPages    = 1;

    if (HAL_FLASHEx_Erase(&erase, &error) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    HAL_FLASH_Lock();
    return true;
}
