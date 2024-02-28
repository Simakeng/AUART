/**
 * @file auart.h
 * @author simakeng (simakeng@outlook.com)
 * @brief Asynchronous UART driver for STM32F/STM32G
 * @version 0.1
 * @date 2024-02-27
 *
 * @copyright Copyright (c) 2024
 *
 * ==================================
 *           How to use:
 * ==================================
 * 1. Fill the auart_init_t structure and call auart_init() to initialize
 *    the AUART.
 * 2. Call auart_dma_cplt_callback in the corresponding DMA interrupt.
 * 3. Call auart_dma_half_cplt_callback in the corresponding DMA interrupt.
 * 4. Call auart_idle_callback in the corresponding UART interrupt.
 * 5. enjoy the auart_tx and auart_rx functions!
 *
 *
 * @note ALL API in this file has same return value convention:
 *      <0: Error
 *       0: Success
 *      >0: Success, but there is a but...
 *
 *      to find out what that but is, call auart_get_flags()
 */

#include <stdint.h>

// import the configuration file
#include "auart-config.h"

#ifndef __AUART_H__
#define __AUART_H__

typedef struct
{
    void *dma_rx_stream_ndtr_addr;
    int (*dma_rx_start)(void *hdma, void *pdst, uint32_t len);
} auart_init_t;

typedef auart_init_t auart_t;

int auart_dma_cplt_callback(auart_t *hauart);
int auart_dma_half_cplt_callback(auart_t *hauart);
int auart_idle_callback(auart_t *hauart);

void auart_init(void);
int32_t auart_tx(auart_t *device, const void *data, int32_t len);
int32_t auart_rx(auart_t *device, void *data, int32_t len);


#endif // !#ifndef __AUART_H__
