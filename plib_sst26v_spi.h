#ifndef PLIB_MEMORY_SPI_H
#define PLIB_MEMORY_SPI_H

/**
 * @file plib_memory_spi.h
 * @brief Fichier en-tête de bibliothèque de communication SPI de la flash SST26V
 * 
 * Ce fichier contient les fonctions pour communiquer en SPI avec la mémoire flash SST26V
 * @author Ramiro Najera
 * @version 0.1
 * @date 2024-02-21
 * @copyright Copyright (c) 2024
 */

#include "libs/common_c_libs/plib_comm_struct.h"

/** @brief Données fictives pendant la lecture */
#define SST26V_DUMMY_DATA       0xFF

/**
 * @brief Écrit un byte dans un registre du SST26V.
 * @param data Byte à écrire
 */
void SST26V_WriteByte(SPIConfiguration_t *spi, unsigned char* data);

/**
 * @brief Lit un byte dans un registre du SST26V
 * @param data Byte à lire
 */
void SST26V_ReadByte(SPIConfiguration_t *spi, unsigned char* data);

/**
 * @brief Écrit un buffer dans un registre du SST26V.
 * @param data Buffer à écrire
 * @param size Taille de buffer en bytes
 */
void SST26V_WriteBuffer(SPIConfiguration_t *spi, unsigned char* data, unsigned int size);

/**
 * @brief Lit un buffer dans un registre du SST26V
 * @param data Buffer à lire
 * @param size Taille de buffer en bytes
 */
void SST26V_ReadBuffer(SPIConfiguration_t *spi, unsigned char* data, unsigned int size);

#endif  // PLIB_MEMORY_SPI_H