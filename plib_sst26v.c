/**
 * @file plib_sst26v.c
 * @author Ramiro Najera
 * @brief Fichier source principal pour la mémoire flash SST26V
 * 
 * Ce fichier en-tête est le point d'entrée principal pour la bibliothèque de gestion des la mémoire
 * flash SST26V. Il inclut les autres fichiers d'en-tête nécessaires pour la gestion de
 * communication SPI avec la mémoire flash SST26V et le contrôle du Chip Select (CS).
 * @version 1.0.4
 * @date 2025-04-23
 * @copyright Copyright (c) 2025
 */

#include <string.h>
#include "plib_sst26v.h"

// Static functions

static void SST26V_WriteByte(SPI_t *spi, uint8_t* data);
static void SST26V_ReadByte(SPI_t *spi, uint8_t* data);
static void SST26V_WriteBuffer(SPI_t *spi, uint8_t* data, uint16_t size);
static void SST26V_ReadBuffer(SPI_t *spi, uint8_t* data, uint16_t size);
static void SST26V_StartTranmission(SPI_t *spi);
static void SST26V_EndTramission(SPI_t *spi);
static void SST26V_WriteRegister(SPI_t *spi, uint8_t reg);
static void SST26V_WriteData(SPI_t *spi, uint8_t reg, uint8_t* writeData, uint16_t size);
static void SST26V_WriteReadRegister(SPI_t *spi, uint8_t reg, uint16_t size, uint8_t* readData);

// Public API

void SST26V_Init(SST26V_t *obj)
{
    SST26V_EndTramission(&obj->spi);
    SST26V_Reset(obj);
    // unlock write
    SST26V_UnlockWrite(obj);   // maybe to delete: unlock only when writting memory??
}

void SST26V_Reset(SST26V_t *obj)
{
    SST26V_WriteEnableReset(&obj->spi);
    SST26V_WriteReset(&obj->spi);
}

void SST26V_UnlockWrite(SST26V_t *obj)
{
    uint8_t tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SST26V_WriteEnableWriteReg(&obj->spi);
    // populate register ('0' unlock every block)
    memset(tempWriteProtectionRegisters, 0x00, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(uint8_t));
    // set unlock write
    SST26V_WriteBlockProtectionReg(&obj->spi, tempWriteProtectionRegisters); 
}

void SST26V_LockWrite(SST26V_t *obj)
{
    uint8_t tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SST26V_WriteEnableWriteReg(&obj->spi);
    // populate register ('1' lock every block)
    memset(tempWriteProtectionRegisters, 0xFF, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(uint8_t));
    // set lock write
    SST26V_WriteBlockProtectionReg(&obj->spi, tempWriteProtectionRegisters);  
}

uint8_t SST26V_WaitBusy(SPI_t *spi, uint32_t timeout)
{
    uint8_t status;
    uint32_t elapsed = 0;

    while (1)
    {
        SST26V_ReadStatusReg(spi, &status);
        elapsed++;

        if ((status & 0x80) == 0)
            return 0;

        if (elapsed >= timeout)
            return 1;
    }
}

void SST26V_Erase4KBSector(SPI_t *spi, uint32_t address)
{
    uint8_t tmpData[4];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register and address
    tmpData[0] = SST26V_SE;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    SST26V_WriteBuffer(spi, tmpData, 4);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_ReadMemory(SPI_t *spi, uint32_t address, uint16_t size, uint8_t* data)
{
    uint8_t tmpData[4];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register and start address
    tmpData[0] = SST26V_READ;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    SST26V_WriteBuffer(spi, tmpData, 4);
    // Read data (size)
    SST26V_ReadBuffer(spi, data, size);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_WriteMemory(SPI_t *spi, uint8_t* data, uint32_t address, uint16_t size)
{
    uint8_t tmpData[4];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register and address
    tmpData[0] = SST26V_PP;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    SST26V_WriteBuffer(spi, tmpData, 4);
    // Send data to be written
    SST26V_WriteBuffer(spi, data, size);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_EraseMore4KBSector(SPI_t *spi, uint32_t address)
{
    uint8_t tmpData[4];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register and address
    tmpData[0] = SST26V_BE;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    SST26V_WriteBuffer(spi, tmpData, 4);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_EraseAll(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_CE);
}

void Memory_WriteDisableReset(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_NOP);
}

void SST26V_WriteEnableReset(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_RSTEN);
}

void SST26V_WriteReset(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_RST);
}

void SST26V_WriteStatusReg(SPI_t *spi, uint8_t* regs)
{
    SST26V_WriteData(spi, SST26V_WRSR, regs, 2);
}

void SST26V_WriteBurstLenReg(SPI_t *spi, uint8_t length)
{
    uint8_t tmpData[1];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register and configuration
    tmpData[0] = SST26V_SB;
    SST26V_WriteByte(spi, tmpData);
    SST26V_WriteByte(spi, &length);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_WriteEnableWriteReg(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_WREN);
}

void SST26V_WriteDisableWriteReg(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_WRDI);
}

void SST26V_WriteSuspendWrite(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_WRSU);
}

void SST26V_WriteResumeWrite(SPI_t *spi)
{
    SST26V_WriteRegister(spi, SST26V_WRRE);
}

void SST26V_WriteBlockProtectionReg(SPI_t *spi, uint8_t* regs)
{
    SST26V_WriteData(spi, SST26V_WBPR, regs, 18);
}

void SST26V_ReadStatusReg(SPI_t *spi, uint8_t* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RDSR, 1, data);
}

void SST26V_ReadConfigurationReg(SPI_t *spi, uint8_t* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RDCR, 1, data);
}

void SST26V_ReadJEDECIdReg(SPI_t *spi, uint8_t* data)
{
    SST26V_WriteReadRegister(spi, SST26V_JEDEC_ID, 3, data);
}

void SST26V_ReadBlockProtectionReg(SPI_t *spi, uint8_t* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RBPR, 18, data);
}

// Static functions

static void SST26V_WriteByte(SPI_t *spi, uint8_t* data)
{
    if(spi->Write)
        spi->Write(data, 1);
}

static void SST26V_ReadByte(SPI_t *spi, uint8_t* data)
{
    if(spi->Read)
        spi->Read(data, 1);
}

static void SST26V_WriteBuffer(SPI_t *spi, uint8_t* data, uint16_t size)
{
    for(uint8_t i = 0; i < size; i++)
        SST26V_WriteByte(spi, &data[i]);
}

static void SST26V_ReadBuffer(SPI_t *spi, uint8_t* data, uint16_t size)
{
    for(uint8_t i = 0; i < size; i++)
        SST26V_ReadByte(spi, &data[i]);
}

static void SST26V_StartTranmission(SPI_t *spi)
{
    if(spi->pinEN.Clear != NULL)
        spi->pinEN.Clear();
    if(spi->pinCS.Clear != NULL) 
        spi->pinCS.Clear();
}

static void SST26V_EndTramission(SPI_t *spi)
{
    if(spi->pinCS.Set != NULL)
        spi->pinCS.Set();
    if(spi->pinEN.Set != NULL)
        spi->pinEN.Set();
}

static void SST26V_WriteRegister(SPI_t *spi, uint8_t reg)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_EndTramission(spi);
}

static void SST26V_WriteData(SPI_t *spi, uint8_t reg, uint8_t* writeData, uint16_t size)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_WriteBuffer(spi, writeData, size);
    SST26V_EndTramission(spi);
}

static void SST26V_WriteReadRegister(SPI_t *spi, uint8_t reg, uint16_t size, uint8_t* readData)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_ReadBuffer(spi, readData, size);
    SST26V_EndTramission(spi);
}