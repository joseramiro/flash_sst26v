#ifndef PLIB_MEMORY_SPI_H
#define PLIB_MEMORY_SPI_H

/**
 * @file plib_sst26v_spi.h
 * @brief Fichier en-tête de bibliothèque de communication SPI de la flash SST26V
 * 
 * Ce fichier contient les fonctions pour communiquer en SPI avec la mémoire flash SST26V
 * @author Ramiro Najera
 * @version 1.0.0
 * @date 2025-04-23
 * @copyright Copyright (c) 2025
 */

#include "libs/common_c_libs/plib_comm_struct.h"

/** @brief Données fictives pendant la lecture */
#define SST26V_DUMMY_DATA       0xFF

/**
 * @brief Écrit un byte dans un registre du SST26V.
 * @param spi Configuration de communication SPI
 * @param data Byte à écrire
 */
void SST26V_WriteByte(SPI_t *spi, unsigned char* data);

/**
 * @brief Lit un byte dans un registre du SST26V
 * @param spi Configuration de communication SPI
 * @param data Byte à lire
 */
void SST26V_ReadByte(SPI_t *spi, unsigned char* data);

/**
 * @brief Ecrit un buffer au SST26V
 * @param spi Configuration de communication SPI
 * @param data Buffer à envoyer
 * @param size Taille de buffer en bytes
 */
void SST26V_WriteBuffer(SPI_t *spi, unsigned char* data, unsigned int size);

/**
 * @brief Lit un buffer dans un registre du SST26V
 * @param spi Configuration de communication SPI
 * @param data Buffer à lire
 * @param size Taille de buffer en bytes
 */
void SST26V_ReadBuffer(SPI_t *spi, unsigned char* data, unsigned int size);

#endif  // PLIB_MEMORY_SPI_H