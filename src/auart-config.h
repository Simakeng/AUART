/**
 * @file auart-config.h
 * @author simakeng (simakeng@outlook.com)
 * @brief Configuration file for the AUART driver
 * @version 0.1
 * @date 2024-02-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __AUART_CONFIG_H__
#define __AUART_CONFIG_H__

// * All the configuration options can be overridden
// * just inlcude your own configuration file here.
// *
// * #include "config.h"

#ifndef CONFIG_AUART_TX_BUFFER_SIZE
/**
 * @brief How many bytes should be reserved for the TX buffer
 * @note This value is nice to be a power of 2.
 */
#define CONFIG_AUART_TX_BUFFER_SIZE 128
#endif // !#ifndef CONFIG_AUART_TX_BUFFER_SIZE

#ifndef CONFIG_AUART_RX_BUFFER_SIZE
/**
 * @brief How many bytes should be reserved for the RX buffer
 * @note This value is nice to be a power of 2.
 */
#define CONFIG_AUART_RX_BUFFER_SIZE 1024
#endif // !#ifndef CONFIG_AUART_RX_BUFFER_SIZE

#ifndef CONFIG_AUART_USE_TIME_API
/**
 * @brief Whether to use the time API or not.
 * If set to 1
 */
#define CONFIG_AUART_USE_TIME_API 1
#endif // !#ifndef CONFIG_AUART_USE_TIME_API

/**
 * ==================================
 *           Error Codes
 * ==================================
 * You can override the error codes here.
 */
#define AUART_OK 0
#define AUART_ERROR -1
#define AUART_TIMEOUT -2
#define AUART_INVALID_ARGUMENT -3
#define AUART_BUSY -4
#define AUART_NOT_INITIALIZED -5
#define AUART_NOT_SUPPORTED -6

#endif // !#ifndef __AUART_CONFIG_H__
