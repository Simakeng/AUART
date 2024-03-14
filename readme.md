# Asynchronous Full Duplex UART Library for Embedded MCUs.

- DMA based implementation
- Minimalistic API
  - `int32_t auart_tx(auart_t *device, const void *data, int32_t len);`
  - `int32_t auart_rx(auart_t *device, void *data, int32_t len);`
- Non-blocking IO

## Chip Support
- [X] STM32F4
- [ ] STM32F3
- [ ] STM32F1
- [ ] STM32G0  (WIP)
- [ ] ESP32-C3 (maybe)