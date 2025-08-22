/**
 * @file plib_sst26v.c
 * @author Ramiro Najera
 * @brief Fichier source principal pour la mémoire flash SST26V
 * 
 * Ce fichier en-tête est le point d'entrée principal pour la bibliothèque de gestion des la mémoire
 * flash SST26V. Il inclut les autres fichiers d'en-tête nécessaires pour la gestion de
 * communication SPI avec la mémoire flash SST26V et le contrôle du Chip Select (CS).
 * @version 1.0.1
 * @date 2025-04-23
 * @copyright Copyright (c) 2025
 */

#include <string.h>
#include "plib_sst26v_spi.h"
#include "plib_sst26v.h"

/* ==== Fonctions SPI ==== */

void SST26V_StartTranmission(SPI_t *spi)
{
    if(spi->en.clear != NULL)
        spi->en.clear();
    if(spi->cs.clear != NULL) 
        spi->cs.clear();
}

void SST26V_EndTramission(SPI_t *spi)
{
    if(spi->cs.set != NULL)
        spi->cs.set();
    if(spi->en.set != NULL)
        spi->en.set();
}

void SST26V_SetWriteProtectionHW(SST26V_t *conf)
{
    if(spi->wp.set != NULL)
        conf->wp.set();
}

void SST26V_ClearWriteProtectionHW(SST26V_t *conf)
{
    if(spi->wp.clear != NULL)
        conf->wp.clear();
}

void SST26V_SetHoldingHW(SST26V_t *conf)
{
    if(spi->hold.set != NULL)
        conf->hold.set();
}

void SST26V_ClearHoldingHW(SST26V_t *conf)
{
    if(spi->hold.clear != NULL)
        conf->hold.clear();
}

/* ==== Fonctions de configuration ==== */

void SST26V_Init(SST26V_t *conf)
{
    SST26V_EndTramission(&conf->spi);
    SST26V_SetHoldingHW(conf);            // disabled when set to 1
    SST26V_SetWriteProtectionHW(conf);    // disabled when set to 1
    SST26V_Reset(conf);
    // unlock write
    SST26V_UnlockWrite(conf);   // maybe to delete: unlock only when writting memory??
}

void SST26V_Reset(SST26V_t *conf)
{
    SST26V_WriteEnableReset(&conf->spi);
    SST26V_WriteReset(&conf->spi);
}

void SST26V_UnlockWrite(SST26V_t *conf)
{
    unsigned char tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SST26V_WriteEnableWriteReg(&conf->spi);
    // populate register ('0' unlock every block)
    memset(tempWriteProtectionRegisters, 0x00, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(unsigned char));
    // set unlock write
    SST26V_WriteBlockProtectionReg(&conf->spi, tempWriteProtectionRegisters); 
}

void SST26V_LockWrite(SST26V_t *conf)
{
    unsigned char tempWriteProtectionRegisters[SST26V_NUM_BYTES_PROTECTION_REG];
    // enable write
    SST26V_WriteEnableWriteReg(&conf->spi);
    // populate register ('1' lock every block)
    memset(tempWriteProtectionRegisters, 0xFF, SST26V_NUM_BYTES_PROTECTION_REG * sizeof(unsigned char));
    // set lock write
    SST26V_WriteBlockProtectionReg(&conf->spi, tempWriteProtectionRegisters);  
}

void SST26V_Erase4KBSector(SPI_t *spi, unsigned long address)
{
    unsigned char tmpData[4];
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

void SST26V_ReadMemory(SPI_t *spi, unsigned long address, unsigned int size, unsigned char* data)
{
    unsigned char tmpData[4];
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

void SST26V_WriteMemory(SPI_t *spi, unsigned char* data, unsigned long address, unsigned int size)
{
    unsigned char tmpData[4];
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

void SST26V_EraseMore4KBSector(SPI_t *spi, unsigned long address)
{
    unsigned char tmpData[4];
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

/* ==== Fonctions d'écriture ==== */

void SST26V_WriteRegister(SPI_t *spi, unsigned char reg)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_EndTramission(spi);
}

void SST26V_WriteData(SPI_t *spi, unsigned char reg, unsigned char* writeData, unsigned int size)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_WriteBuffer(spi, writeData, size);
    SST26V_EndTramission(spi);
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

void SST26V_WriteStatusReg(SPI_t *spi, unsigned char* regs)
{
    SST26V_WriteData(spi, SST26V_WRSR, regs, 2);
}

void SST26V_WriteBurstLenReg(SPI_t *spi, unsigned char length)
{
    unsigned char tmpData[1];
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

void SST26V_WriteBlockProtectionReg(SPI_t *spi, unsigned char* regs)
{
    SST26V_WriteData(spi, SST26V_WBPR, regs, 18);
}

/* ==== Fonctions de lecture ==== */

void SST26V_WriteReadRegister(SPI_t *spi, unsigned char reg, unsigned int size, unsigned char* readData)
{
    SST26V_StartTranmission(spi);
    SST26V_WriteByte(spi, &reg);
    SST26V_ReadBuffer(spi, readData, size);
    SST26V_EndTramission(spi);
}

void SST26V_ReadStatusReg(SPI_t *spi, unsigned char* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RDSR, 1, data);
}

void SST26V_ReadConfigurationReg(SPI_t *spi, unsigned char* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RDCR, 1, data);
}

void SST26V_ReadJEDECIdReg(SPI_t *spi, unsigned char* data)
{
    unsigned char tmpData[1];
    // Start tranmission
    SST26V_StartTranmission(spi);
    // Send register
    tmpData[0] = SST26V_JEDEC_ID;
    SST26V_WriteByte(spi, tmpData);
    // Read register 
    SST26V_ReadBuffer(spi, data, 3);
    // Stop tranmission
    SST26V_EndTramission(spi);
}

void SST26V_ReadBlockProtectionReg(SPI_t *spi, unsigned char* data)
{
    SST26V_WriteReadRegister(spi, SST26V_RBPR, 18, data);
}