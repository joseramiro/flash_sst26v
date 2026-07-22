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

/** @brief Nombre de bytes dans "Proteced Block Register" */
#define SST26V_NUM_BYTES_PROTECTION_REG     18

/**
 * @brief Temps maximum pour réaliser une écriture
 */
#define WRITE_TIMEOUT      1000

/**
 * @brief Identifiant du fabriquant
 */
#define MANUFACTURER_ID     0xBF

/**
 * @brief Identifiant du type de périphérique
 */
#define DEVICE_TYPE         0x26

/**
 * @brief Identifiant du produit
 */
#define DEVICE_ID           0x43

/*==============================================================================
 * Private function prototypes
 *============================================================================*/

static void Write(SPI_t *spi, uint8_t* data, const uint16_t size);
static void Read(SPI_t *spi, uint8_t* data, const uint16_t size);
static void StartTranmission(SPI_t *spi);
static void EndTramission(SPI_t *spi);
static void WriteInstruction(SPI_t *spi, uint8_t reg);
static void WriteRegister(SPI_t *spi, uint8_t reg, uint8_t* writeData, const uint16_t size);
static void ReadRegister(SPI_t *spi, uint8_t reg, const uint16_t size, uint8_t* readData);
static void Unlock(SST26V_t *obj);
//static void Lock(SST26V_t *obj);
static void SendEnableReset(SPI_t *spi);
static void SendReset(SPI_t *spi);
static void Reset(SST26V_t *obj);
static void SendEnableWrite(SPI_t *spi);
static void SendDisableWrite(SPI_t *spi);
static void SendProtectedWriteBlocks(SPI_t *spi, uint8_t* regs);
static void ReadStatus(SPI_t *spi, uint8_t* data);
static void ReadId(SPI_t *spi, uint8_t* data);
static uint8_t WaitBusy(SPI_t *spi, uint32_t timeout);
static void SendEraseChip(SPI_t *spi);
static void SendEraseSector(SPI_t *spi, uint32_t address);
static void SendWriteMemory(SPI_t *spi, uint8_t* data, uint32_t address, uint16_t size);
static void SendReadMemory(SPI_t *spi, uint32_t address, uint16_t size, uint8_t* data);
static uint8_t CheckId(SPI_t *spi);

/*==============================================================================
 * Private data
 *============================================================================*/

/*==============================================================================
 * Public functions
 *============================================================================*/

uint8_t SST26V_Init(SST26V_t *obj)
{
    // Init sequence
    EndTramission(&obj->spi);
    Reset(obj);
    Unlock(obj);
    return(CheckId(&obj->spi));
}

void SST26V_ReadMemory(SST26V_t *obj, uint32_t address, uint16_t size, uint8_t* data)
{
    SendReadMemory(&obj->spi, address, size, data);
}

uint8_t SST26V_WriteMemory(SST26V_t *obj, uint8_t* data, uint32_t address, uint16_t size)
{
     // Wait flash to be available otherwise return error code
    if(WaitBusy(&obj->spi, WRITE_TIMEOUT))
        return 1;
    // Write flash data
    SendEnableWrite(&obj->spi);
    SendWriteMemory(&obj->spi, data, address, size);
    SendDisableWrite(&obj->spi);
    return 0;
}

uint8_t SST26V_EraseSector(SST26V_t *obj, uint32_t address)
{
    // Wait flash to be available otherwise return error code
    if(WaitBusy(&obj->spi, WRITE_TIMEOUT))
        return 1;
    // populate current flash array with read data
    SendEnableWrite(&obj->spi);
    SendEraseSector(&obj->spi, address);
    SendDisableWrite(&obj->spi);
    return 0;
}

uint8_t SST26V_EraseChip(SST26V_t *obj)
{
    // Wait flash to be available otherwise return error code
    if(WaitBusy(&obj->spi, WRITE_TIMEOUT))
        return 1;
    // populate current flash array with read data
    SendEnableWrite(&obj->spi);
    SendEraseChip(&obj->spi);
    SendDisableWrite(&obj->spi);
    return 0;
}

/*==============================================================================
 * Private functions
 *============================================================================*/

static void Write(SPI_t *spi, uint8_t* data, const uint16_t size)
{
    if(spi->Write)
        spi->Write(data, size);
}

static void Read(SPI_t *spi, uint8_t* data, const uint16_t size)
{
    if(spi->Read)
        spi->Read(data, size);
}

static void StartTranmission(SPI_t *spi)
{
    if(spi->pinEN.Clear != NULL)
        spi->pinEN.Clear();
    if(spi->pinCS.Clear != NULL) 
        spi->pinCS.Clear();
}

static void EndTramission(SPI_t *spi)
{
    if(spi->pinCS.Set != NULL)
        spi->pinCS.Set();
    if(spi->pinEN.Set != NULL)
        spi->pinEN.Set();
}

static void WriteInstruction(SPI_t *spi, uint8_t reg)
{
    StartTranmission(spi);
    Write(spi, &reg, 1);
    EndTramission(spi);
}

static void WriteRegister(SPI_t *spi, uint8_t reg, uint8_t* writeData, const uint16_t size)
{
    StartTranmission(spi);
    Write(spi, &reg, 1);
    Write(spi, writeData, size);
    EndTramission(spi);
}

static void ReadRegister(SPI_t *spi, uint8_t reg, const uint16_t size, uint8_t* readData)
{
    StartTranmission(spi);
    Write(spi, &reg, 1);
    Read(spi, readData, size);
    EndTramission(spi);
}

static void Unlock(SST26V_t *obj)
{
    uint8_t tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SendEnableWrite(&obj->spi);
    // populate register ('0' unlock every block)
    memset(tempWriteProtectionRegisters, 0x00, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(uint8_t));
    // set unlock write
    SendProtectedWriteBlocks(&obj->spi, tempWriteProtectionRegisters); 
}

/*
static void Lock(SST26V_t *obj)
{
    uint8_t tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SendEnableWrite(&obj->spi);
    // populate register ('1' lock every block)
    memset(tempWriteProtectionRegisters, 0xFF, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(uint8_t));
    // set lock write
    SendProtectedWriteBlocks(&obj->spi, tempWriteProtectionRegisters);  
}
*/

static void SendEnableReset(SPI_t *spi)
{
    WriteInstruction(spi, SST26V_RSTEN);
}

static void SendReset(SPI_t *spi)
{
    WriteInstruction(spi, SST26V_RST);
}

static void Reset(SST26V_t *obj)
{
    SendEnableReset(&obj->spi);
    SendReset(&obj->spi);
}

static void SendEnableWrite(SPI_t *spi)
{
    WriteInstruction(spi, SST26V_WREN);
}

static void SendDisableWrite(SPI_t *spi)
{
    WriteInstruction(spi, SST26V_WRDI);
}

static void SendProtectedWriteBlocks(SPI_t *spi, uint8_t* regs)
{
    WriteRegister(spi, SST26V_WBPR, regs, 18);
}

static void ReadStatus(SPI_t *spi, uint8_t* data)
{
    ReadRegister(spi, SST26V_RDSR, 1, data);
}

static void ReadId(SPI_t *spi, uint8_t* data)
{
    ReadRegister(spi, SST26V_JEDEC_ID, 3, data);
}

static uint8_t WaitBusy(SPI_t *spi, uint32_t timeout)
{
    uint8_t status;
    uint32_t elapsed = 0;

    while(1)
    {
        ReadStatus(spi, &status);
        elapsed++;

        if ((status & 0x80) == 0)
            return 0;

        if (elapsed >= timeout)
            return 1;
    }
}

static void SendEraseChip(SPI_t *spi)
{
    WriteInstruction(spi, SST26V_CE);
}

static void SendEraseSector(SPI_t *spi, uint32_t address)
{
    uint8_t tmpData[4];
    // Start tranmission
    StartTranmission(spi);
    // Send register and address
    tmpData[0] = SST26V_SE;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    Write(spi, tmpData, 4);
    // Stop tranmission
    EndTramission(spi);
}

static void SendWriteMemory(SPI_t *spi, uint8_t* data, uint32_t address, uint16_t size)
{
    uint8_t tmpData[4];
    // Start tranmission
    StartTranmission(spi);
    // Send register and address
    tmpData[0] = SST26V_PP;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    Write(spi, tmpData, 4);
    // Send data to be written
    Write(spi, data, size);
    // Stop tranmission
    EndTramission(spi);
}

static void SendReadMemory(SPI_t *spi, uint32_t address, uint16_t size, uint8_t* data)
{
    uint8_t tmpData[4];
    // Start tranmission
    StartTranmission(spi);
    // Send register and start address
    tmpData[0] = SST26V_READ;
    tmpData[1] = (address >> 16);
    tmpData[2] = (address >> 8);
    tmpData[3] = (address);
    Write(spi, tmpData, 4);
    // Read data (size)
    Read(spi, data, size);
    // Stop tranmission
    EndTramission(spi);
}

static uint8_t CheckId(SPI_t *spi)
{
    uint8_t id[3];
    ReadId(spi, id);
    if(id[0] == MANUFACTURER_ID && id[1] == DEVICE_TYPE && id[2] == DEVICE_ID)
        return 0;
    return 1;
}