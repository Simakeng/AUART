#include "auart.h"
#include <string.h>

#define AUART_TX_DMA_STOPED 0

int auart_init(auart_t *hauart, auart_init_t *init)
{
    // argument sanity checks
    if (hauart == NULL || init == NULL)
        return AUART_INVALID_ARGUMENT;

    if (init->dma_rx_start == NULL ||
        init->dma_rx_abort == NULL ||
        init->dma_rx_update_progress == NULL)
        return AUART_INVALID_ARGUMENT;

    if (init->dma_tx_abort == NULL ||
        init->dma_tx_start == NULL)
        return AUART_INVALID_ARGUMENT;

#if (CONFIG_AUART_USE_TIME_API == 1)
    if (init->get_tick_ms == NULL)
        return AUART_INVALID_ARGUMENT;
#endif

    // clear the device
    memset(hauart, 0, sizeof(auart_t));

    // copy datas
    hauart->op = *init;

    // start the rx dma
    int res = hauart->op.dma_rx_start(
        hauart,
        hauart->rx_buffer,
        CONFIG_AUART_RX_BUFFER_SIZE);

    if (res < 0)
        return res;

    return AUART_OK;
}

static inline int32_t __auart_get_capacity_in_tx_buffer(auart_t *hauart)
{
    int32_t size_in_buffer = CONFIG_AUART_TX_BUFFER_SIZE;
    size_in_buffer -= hauart->tx_head;
    size_in_buffer += hauart->tx_tail;
    size_in_buffer %= CONFIG_AUART_TX_BUFFER_SIZE;

    return size_in_buffer;
}

static inline int32_t __auart_get_data_size_in_tx_buffer(auart_t *hauart)
{
    int32_t size_in_buffer = __auart_get_capacity_in_tx_buffer(hauart);
    int32_t data_len = CONFIG_AUART_TX_BUFFER_SIZE - 1 - size_in_buffer;

    return data_len;
}

static inline int __auart_tx_dma_continue(auart_t *hauart)
{
    //? this function is in IRQ context ?//
    //? this function is in thread context ?//

    int32_t tx_tail = hauart->tx_tail;
    int32_t tx_head = hauart->tx_head;

    // check if the dma is already started
    if (hauart->tx_dma.is_started)
        return AUART_OK;

    // check if there is anything to send
    if (tx_head == tx_tail)
        return AUART_OK; // nope

    int32_t num_byte_to_send = 0;

    if (tx_head < tx_tail)
        num_byte_to_send = tx_tail - tx_head;
    else
        num_byte_to_send = CONFIG_AUART_TX_BUFFER_SIZE - tx_head;

    uint8_t *pdata = hauart->tx_buffer + tx_head;

    // start the dma
    int res = hauart->op.dma_tx_start(
        hauart->op.h_txdma,
        pdata,
        num_byte_to_send);

    if (res < 0)
        return res;
    
    hauart->tx_dma.commited_size = num_byte_to_send;
    return 0;
}

int auart_tx_cplt_callback(auart_t *hauart)
{
    //? this function is in IRQ context ?//

    // update the head
    int32_t new_head = hauart->tx_head;
    new_head += hauart->tx_dma.commited_size;
    new_head %= CONFIG_AUART_TX_BUFFER_SIZE;

    hauart->tx_head = new_head;

    // stop the dma
    hauart->tx_dma.is_started = AUART_TX_DMA_STOPED;

    int32_t need_transfer = __auart_get_data_size_in_tx_buffer(hauart);
    if (need_transfer)
        return __auart_tx_dma_continue(hauart);

    return 0;
}

int auart_tx(auart_t *hauart, const void *data, int32_t len)
{
    uint8_t *pu8data = (uint8_t *)data;
    int32_t size_in_buffer = __auart_get_capacity_in_tx_buffer(hauart);

    int32_t size_available = CONFIG_AUART_TX_BUFFER_SIZE - 1;
    size_available -= size_in_buffer;

    int32_t size_to_copy = len;
    if (size_to_copy > size_available)
        size_to_copy = size_available;

    if (size_available <= 0)
        return 0;

    int32_t new_tail = (hauart->tx_tail + size_to_copy);
    new_tail %= CONFIG_AUART_TX_BUFFER_SIZE;

    int32_t size_to_end = CONFIG_AUART_TX_BUFFER_SIZE;
    size_to_end = size_to_end - hauart->tx_tail;

    int32_t size_first_copy = size_to_copy;
    if (size_first_copy > size_to_end)
        size_first_copy = size_to_end;

    memcpy(hauart->tx_buffer + hauart->tx_tail, data, size_first_copy);

    int32_t size_second_copy = size_to_copy - size_first_copy;
    if (size_second_copy == 0)
        goto copy_done;

    memcpy(hauart->tx_buffer, pu8data + size_first_copy, size_second_copy);

copy_done:
    hauart->tx_tail = new_tail;

    if (!hauart->tx_dma.is_started)
        __auart_tx_dma_continue(hauart);

    return size_to_copy;
}

int auart_rx(auart_t *hauart, void *data, int32_t len);

int auart_idle_callback(auart_t *hauart)
{
    return 0;
}

int auart_dma_rx_half_cplt_callback(auart_t *hauart)
{
    return 0;
}

int auart_dma_rx_cplt_callback(auart_t *hauart)
{
    return 0;
}
