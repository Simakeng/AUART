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
 *      >0: Success, see the function documentation for more details.

 */

#include <stdint.h>
#include <stdbool.h>

// import the configuration file
#include "auart-config.h"

#ifndef __AUART_H__
#define __AUART_H__

/**
 * @brief The initialization structure of the AUART
 */
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

    /**
     * @brief this callback is used by the driver to start the TX DMA.
     *
     * @param hdma the handle of the DMA
     * @param psrc the source buffer
     * @param len the number of bytes to be sent
     *
     * @return <0: Error, =0: Success
     *
     */
    int (*dma_tx_start)(void *hdma, const void *psrc, uint32_t len);

    /**
     * @brief this callback is used by the driver to stop the TX DMA.
     *
     * @param hdma the handle of the DMA
     *
     * @return <0: Error, =0: Success
     *
     * @note this function is called by the driver in the `auart_deinit()`
     * function.
     */
    int (*dma_tx_abort)(void *hdma);

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
    uint32_t (*get_tick_ms)(void);
#endif

    void *h_rxdma;
    void *h_txdma;

} auart_init_t;

/**
 * @brief The AUART device structure
 * @warning User should not access the members of this structure directly.
 */
typedef struct
{
    uint8_t tx_buffer[CONFIG_AUART_TX_BUFFER_SIZE];
    uint8_t rx_buffer[CONFIG_AUART_RX_BUFFER_SIZE];

    volatile uint32_t tx_head; // rw by DMA and IRQ, ro by api
    volatile uint32_t tx_tail; // ro by DMA and IRQ, rw by api

    volatile uint32_t rx_head; // ro by DMA and IRQ, rw by api
    volatile uint32_t rx_tail; // rw by DMA and IRQ, ro by api

    /**
     * This union 'flags' is to make the flag operations atomic.
     *
     * if the value of such flag is 0, it means the DMA is not started.
     * otherwise, it represents the number of bytes of last dma_start's
     * `len` parameter.
     */
    volatile union
    {
        int32_t is_started;
        int32_t commited_size;
    } tx_dma;

    bool rx_dma_started;

    auart_init_t op;
} auart_t;

/**
 * @brief Auart DMA transfer complete callback.
 *
 * User shloud call this function in the corresponding DMA interrupt.
 *
 * @param hauart the AUART handle
 * @return int <0: Error, =0: Success
 */
int auart_dma_rx_cplt_callback(auart_t *hauart);

/**
 * @brief AUART DMA half transfer complete callback.
 *
 * User shloud call this function in the corresponding DMA interrupt.
 *
 * @param hauart the AUART handle
 * @return int <0: Error, =0: Success
 */
int auart_dma_rx_half_cplt_callback(auart_t *hauart);

/**
 * @brief AUART IDLE interrupt callback.
 *
 * User shloud call this function in the corresponding UART interrupt.
 *
 * @param hauart the AUART handle
 * @return int <0: Error, =0: Success
 */
int auart_idle_callback(auart_t *hauart);

/**
 * @brief AUART DMA transfer complete callback.
 *
 * User shloud call this function in the corresponding DMA interrupt.
 *
 * @param hauart the AUART handle
 * @return int <0: Error, =0: Success
 */
int auart_tx_cplt_callback(auart_t *hauart);

/**
 * @brief Initialize the AUART Driver
 *
 * @param hauart the AUART handle
 * @param init the initialization structure
 * @return <0: Error, =0: Success
 */
int auart_init(auart_t *hauart, auart_init_t *init);

/**
 * @brief Send data to UART Port.
 *
 * @param hauart the AUART handle
 * @param data to be sent
 * @param len how many bytes to be sent
 * @return int <0: Error, otherwise the number of bytes sent
 */
int auart_tx(auart_t *hauart, const void *data, int32_t len);

/**
 * @brief Read data from UART Port.
 *
 * @param hauart the AUART handle
 * @param data the buffer to store the received data
 * @param len how many bytes can be received
 * @return int <0: Error, otherwise the number of bytes received
 */
int auart_rx(auart_t *hauart, void *data, int32_t len);

/**
 * @brief Wait all the data in the TX buffer to be sent.
 *
 * @param hauart the AUART handle
 * @return int <0: Error, =0: Success
 */
int auart_tx_flush(auart_t *hauart);

#endif // !#ifndef __AUART_H__
