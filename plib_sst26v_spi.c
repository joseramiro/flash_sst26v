/**
 * @file plib_memory_spi.c
 * @brief Fichier source de bibliothèque de communication SPI de la flash SST26V
 * 
 * Ce fichier contient les fonctions pour communiquer en SPI avec la mémoire flash SST26V
 * @author Ramiro Najera
 * @version 0.1
 * @date 2024-02-21
 * @copyright Copyright (c) 2024
 */

#include "plib_sst26v_spi.h"
#include "peripheral/spi/spi_master/plib_spi2_master.h"
#include "peripheral/spi/spi_master/plib_spi1_master.h"

void SST26V_WriteByte(SPIConfiguration_t *spi, unsigned char* data)
{
    if(spi->channel == SPI_CH1)
        SPI1_Write(data, 1);
    else
        SPI2_Write(data, 1);
}

void SST26V_ReadByte(SPIConfiguration_t *spi, unsigned char* data)
{
    if(spi->channel == SPI_CH1)
        SPI1_Read(data, 1);
    else
        SPI2_Read(data, 1);
}

void SST26V_WriteBuffer(SPIConfiguration_t *spi, unsigned char* data, unsigned int size)
{
    unsigned int i = 0;

    if(spi->channel == SPI_CH1)
    {
        for(i = 0; i < size; i++)
            SPI1_Write(&data[i], 1);
    }
    else
    {
        for(i = 0; i < size; i++)
            SPI2_Write(&data[i], 1);
    }
}

void SST26V_ReadBuffer(SPIConfiguration_t *spi, unsigned char* data, unsigned int size)
{
    if(spi->channel == SPI_CH1)
        SPI1_Read(data, size);
    else
        SPI2_Read(data, size); 
}