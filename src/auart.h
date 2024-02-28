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
    /**
     * @brief this callback is uesd by driver to check the progress of the
     * RX DMA. User should implement this function to return the number of
     * bytes left to be received.
     *
     * the total number of bytes is given by the len parameter of the
     * `dma_rx_start()` function.
     *
     * this function will be called by the driver in the DMA half and complete 
     * interrupt, the UART IDLE interrupt and also in the `auart_rx()`
     * function.
     * 
     * @param hdma the handle of the DMA
     * @param out_bytes_left the number of bytes left to be received
     *
     * @return <0: Error, =0: Success
     *
     * @note for example, in STM32 ports, this function can be implemented
     * by reading the NDTR register of the DMA.
     */
    int (*dma_rx_update_progress)(void *hdma, uint32_t *out_bytes_left);

    /**
     * @brief this callback is used by the driver to start the RX DMA.
     * 
     * @param hdma the handle of the DMA
     * @param pdst the destination buffer
     * @param len the number of bytes to be received
     * 
     * @return <0: Error, =0: Success
     * 
     */
    int (*dma_rx_start)(void *hdma, void *pdst, uint32_t len);

    /**
     * @brief this callback is used by the driver to stop the RX DMA.
     * 
     * @param hdma the handle of the DMA
     * 
     * @return <0: Error, =0: Success
     * 
     * @note this function is called by the driver in the `auart_deinit()` 
     * function.
     */
    int (*dma_rx_abort)(void *hdma);


#if (CONFIG_AUART_USE_TIME_API == 1)
    /**
     * @brief This function is used by the driver get current timestamp
     * of the system in milliseconds.
     * 
     * this is used to implement the timeout feature of the driver.
     * 
     * @return int the current timestamp in milliseconds
     * 
     * @note the timeout feature can be disabled by
     * setting the CONFIG_AUART_USE_TIME_API to 0.
     */
    int (*get_tick_ms)(void);
#endif

    void *h_rxdma;
    void *h_txdma;

} auart_init_t;

typedef struct
{
    uint8_t tx_buffer[CONFIG_AUART_TX_BUFFER_SIZE];
    uint8_t rx_buffer[CONFIG_AUART_RX_BUFFER_SIZE];

    uint32_t tx_head; // rw by DMA and IRQ, ro by api
    uint32_t tx_tail; // ro by DMA and IRQ, rw by api

    uint32_t rx_head; // ro by DMA and IRQ, rw by api
    uint32_t rx_tail; // rw by DMA and IRQ, ro by api

    auart_init_t op;
} auart_t;

int auart_dma_cplt_callback(auart_t *hauart);

int auart_dma_half_cplt_callback(auart_t *hauart);

int auart_idle_callback(auart_t *hauart);

/**
 * @brief Initialize the AUART Driver
 *
 * @param hauart the AUART handle
 * @param init the initialization structure
 * @return <0: Error, =0: Success, >0: call auart_get_flags()
 */
int auart_init(auart_t *hauart, auart_init_t *init);

int auart_tx(auart_t *hauart, const void *data, int32_t len);

int auart_rx(auart_t *hauart, void *data, int32_t len);

int auart_tx_flush(auart_t *hauart);

int auart_get_flags(auart_t *hauart);

#endif // !#ifndef __AUART_H__
