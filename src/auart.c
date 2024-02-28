#include "auart.h"
#include <string.h>

volatile struct 
{
    uint8_t tx_buffer[CONFIG_AUART_TX_BUFFER_SIZE];
    uint8_t rx_buffer[CONFIG_AUART_RX_BUFFER_SIZE];

    uint32_t tx_head;   // rw by DMA and IRQ, ro by api
    uint32_t tx_tail;   // ro by DMA and IRQ, rw by api

    uint32_t rx_head;   // ro by DMA and IRQ, rw by api
    uint32_t rx_tail;   // rw by DMA and IRQ, ro by api
} auart;

#include <usart.h>

void auart_init(void)
{
    memset(&auart, 0, sizeof(auart));

    HAL_UART_Receive_DMA(&huart1, auart.rx_buffer, CONFIG_AUART_RX_BUFFER_SIZE);
}
