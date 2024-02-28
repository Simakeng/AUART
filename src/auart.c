#include "auart.h"
#include <string.h>

int auart_init(auart_t *hauart, auart_init_t *init)
{
    // argument sanity checks
    if (hauart == NULL || init == NULL)
        return AUART_INVALID_ARGUMENT;

    if (init->dma_rx_start == NULL)
        return AUART_INVALID_ARGUMENT;

    if (init->dma_rx_update_progress == NULL)
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
