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

/**
 * @brief How many bytes should be reserved for the TX buffer
 * @note This value is nice to be a power of 2.
 */
#define CONFIG_AUART_TX_BUFFER_SIZE 128

/**
 * @brief How many bytes should be reserved for the RX buffer
 * @note This value is nice to be a power of 2.
 */
#define CONFIG_AUART_RX_BUFFER_SIZE 1024


#endif // !#ifndef __AUART_CONFIG_H__
