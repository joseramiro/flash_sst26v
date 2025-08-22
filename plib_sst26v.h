#ifndef PLIB_MEMORY_H
#define PLIB_MEMORY_H

/**
 * @file plib_sst26v.h
 * @author Ramiro Najera
 * @brief Fichier en-tête principal pour la mémoire flash SST26V
 * 
 * Ce fichier en-tête est le point d'entrée principal pour la bibliothèque de gestion des la mémoire
 * flash SST26V. Il inclut les autres fichiers d'en-tête nécessaires pour la gestion de
 * communication SPI avec la mémoire flash SST26V et le contrôle du Chip Select (CS).
 * @version 1.0.2
 * @date 2025-04-23
 * @copyright Copyright (c) 2025
 */

#include "libs/common_c_libs/plib_comm_struct.h"
/**
 * 
 */
/** @defgroup SST26V_Registers Registres du SST26V
 *  @{
 */
/** @brief No Operation: annule uniquement une commande de Réinitialisation Active (RSTEN) */
#define SST26V_NOP          0x00
/** @brief Réinitialisation Active: Réinitialisation système (logicielle) */
#define SST26V_RSTEN        0x66
/** @brief Réinitialisation: Réinitialisation après SST26V_RSTEN */
#define SST26V_RST          0x99
/** @brief Envoie le contenu du registre STATUS*/
#define SST26V_RDSR         0x05
/** @brief Ecrit de nouvelles valeurs dans le registre de configuration*/
#define SST26V_WRSR         0x01
/** @brief Envoient le contenu des registres d'état et de configuration. */
#define SST26V_RDCR         0x35
/** @brief Lecture de la flash */
#define SST26V_READ         0x03
/** @brief Spécifie le nombre d'octets à envoyer lors d'une commande de lecture */
#define SST26V_SB           0xC0
/** @brief  Lit le JEDEC-ID identifie le dispositif comme étant le SST26VF032B/032BA */    
#define SST26V_JEDEC_ID     0x9F
/** @brief Met le bit WriteEnable-Latch du registre d'état à '1' */
#define SST26V_WREN         0x06
/** @brief Met le bit WriteEnable-Latch du registre d'état à '0' */
#define SST26V_WRDI         0x04
/** @brief Efface tous les bits du secteur de 4 Ko sélectionné à '1' */
#define SST26V_SE           0x20
/** @brief Efface tous les bits du bloc sélectionné à '1' */
#define SST26V_BE           0xD8
/** @brief efface tous les bits du dispositif à '1' */
#define SST26V_CE           0xC7
/** @brief Programme jusqu'à 256 octets de données dans la mémoire*/
#define SST26V_PP           0x02
/** @brief Permet à l'hôte de programmer ou de lire tout secteur qui n'était pas en cours d'effacement.*/
#define SST26V_WRSU         0xB0
/** @brief L'instruction Write-Resume redémarre une commande d'écriture*/
#define SST26V_WRRE         0x30
/** @brief L'instruction de lecture du registre de protection de bloc*/
#define SST26V_RBPR         0x72
/** @brief Modifie les données du registre de protection de bloc */
#define SST26V_WBPR         0x42
/** @} */

/** @brief Nombre de bytes dans "Proteced Block Register" */
#define SST26V_NUM_BYTES_PROTECTION_REG     18

/** 
 * @struct SST26V_t
 * @brief Structure de configuration du MCP23S17 
 */
typedef struct
{
    /** @brief Identifiant de la mémoire flash */
    unsigned char id;
    /** @brief Fonction pour Write Protection */
    GPIO_t wp;
    /** @brief Fonction pour Hold */
    GPIO_t hold;
     /** @brief Configuration SPI associée */
    SPI_t spi;
}SST26V_t;

/* ==== Fonctions SPI ==== */

/**
 * @brief Démarre une transmission SPI avec le SST26V.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_StartTranmission(SPI_t *spi);

/**
 * @brief Termine une transmission SPI avec le SST26V.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_EndTramission(SPI_t *spi);

/**
 * @brief Met le pin Write Protection à 1
 * @param obj Configuration de SST26V
 */
void SST26V_SetWriteProtectionHW(SST26V_t *obj);

/**
 * @brief Met le pin Write Protection à 0
 * @param obj Configuration de SST26V
 */
void SST26V_ClearWriteProtectionHW(SST26V_t *obj);

/**
 * @brief Met le pin Holding à 1
 * @param obj Configuration de SST26V
 */
void SST26V_SetHoldingHW(SST26V_t *obj);

/**
 * @brief Met le pin Holding à 0
 * @param obj Configuration de SST26V
 */
void SST26V_ClearHoldingHW(SST26V_t *obj);

/* ==== Fonctions de configuration ==== */

/**
 * @brief Initialise le module SST26V avec la configuration fournie
 * @param obj Pointeur vers la configuration du module
 */
void SST26V_Init(SST26V_t *obj);

/**
 * @brief Effectue une réinitialisation en appelant les fonctions d'activation et de
 * réinitialisation.
 * @param obj Pointeur vers la configuration du module
 */
void SST26V_Reset(SST26V_t *obj);

/**
 * @brief Débloque l'écriture de protection
 * @param obj Pointeur vers la configuration du module
 */
void SST26V_UnlockWrite(SST26V_t *obj);

/**
 * @brief Bloque l'écriture de protection
 * @param obj Pointeur vers la configuration du module
 */
void SST26V_LockWrite(SST26V_t *obj);

/**
 * @brief Efface un secteur de mémoire de 4 Ko à l'adresse spécifiée.
 * @param spi Pointeur vers la configuration SPI
 * @param address Adresse du secteur de mémoire à effacer
 */
void SST26V_Erase4KBSector(SPI_t *spi, unsigned long address);

/**
 * @brief Lit des données depuis la mémoire flash.
 * @param spi Pointeur vers la configuration SPI
 * @param address Adresse de départ où les données seront lues
 * @param size Taille des données à lire
 * @param data Tampon pour stocker les données lues
 */
void SST26V_ReadMemory(SPI_t *spi, unsigned long address, unsigned int size, unsigned char* data);

/**
 * @brief Écrit des données dans la mémoire flash. Il est important que l'effacement soit effectué
 * avant d'écrire de nouvelles données.
 * @param spi Pointeur vers la configuration SPI
 * @param data Tampon contenant les données à écrire
 * @param address Adresse de départ des données
 * @param size Nombre d'octets à écrire
 */
void SST26V_WriteMemory(SPI_t *spi, unsigned char* data, unsigned long address, unsigned int size);

/**
 * @brief Efface un secteur de mémoire plus grand que 4 Ko à l'adresse spécifiée.
 * @param spi Pointeur vers la configuration SPI
 * @param address Adresse du secteur de mémoire à effacer
 */
void SST26V_EraseMore4KBSector(SPI_t *spi, unsigned long address);

/**
 * @brief Efface toute la mémoire.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_EraseAll(SPI_t *spi);

/* ==== Fonctions d'écriture ==== */

/**
 * @brief Écrit un registre (1 octet) via SPI. Aucune réponse n'est attendue.
 * @param spi Pointeur vers la configuration SPI
 * @param reg Registre à envoyer (1 octet)
 */
void SST26V_WriteRegister(SPI_t *spi, unsigned char reg);

/**
 * @brief Écrit un registre (1 octet) et des données supplémentaires via SPI. Aucune réponse n'est
 * attendue.
 * @param spi Pointeur vers la configuration SPI
 * @param reg Registre à envoyer (1 octet)
 * @param writeData Données à envoyer
 * @param size Taille des données à envoyer
 */
void SST26V_WriteData(SPI_t *spi, unsigned char reg, unsigned char* writeData, unsigned int size);

/**
 * @brief Annule une séquence de réinitialisation (après activation de la réinitialisation).
 * @param spi Pointeur vers la configuration SPI
 */
void Memory_WriteDisableReset(SPI_t *spi);

/**
 * @brief Active une réinitialisation.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteEnableReset(SPI_t *spi);

/**
 * @brief Réinitialise le dispositif. La réinitialisation doit être activée au préalable.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteReset(SPI_t *spi);

/**
 * @brief Écrit dans les registres de configuration et de statut.
 * @param spi Pointeur vers la configuration SPI
 * @param regs Tampon contenant les données pour les registres de configuration (1 octet) et de
 * statut (1 octet)
 */
void SST26V_WriteStatusReg(SPI_t *spi, unsigned char* regs);

/**
 * @brief Définit la longueur de la rafale de lecture des données.
 * @param spi Pointeur vers la configuration SPI
 * @param length Longueur de la rafale de données
 */
void SST26V_WriteBurstLenReg(SPI_t *spi, unsigned char length);

/**
 * @brief Active les opérations d'écriture.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteEnableWriteReg(SPI_t *spi);

/**
 * @brief Désactive les opérations d'écriture.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteDisableWriteReg(SPI_t *spi);

/**
 * @brief Suspends the write operation for flash memory.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteSuspendWrite(SPI_t *spi);

/**
 * @brief Reprend l'opération d'écriture pour la mémoire flash.
 * @param spi Pointeur vers la configuration SPI
 */
void SST26V_WriteResumeWrite(SPI_t *spi);

/**
 * @brief Écrit dans les registres de protection de bloc.
 * @param spi Pointeur vers la configuration SPI
 * @param regs Tampon contenant les données des registres de protection de bloc (18 octets)
 */
void SST26V_WriteBlockProtectionReg(SPI_t *spi, unsigned char* regs);

/* ==== Fonctions de lecture ==== */

/**
 * @brief Écrit un registre (1 octet) via SPI, puis lit un nombre spécifié d'octets.
 * @param spi Pointeur vers la configuration SPI
 * @param reg Registre à envoyer (1 octet) 
 * @param size Taille des données à lire
 * @param readData Tampon pour stocker les données lues
 */
void SST26V_WriteReadRegister(SPI_t *spi, unsigned char reg, unsigned int size, unsigned char* readData);

/**
 * @brief Lit le registre de statut.
 * @param spi Pointeur vers la configuration SPI
 * @param data Tampon pour stocker le registre de statut (1 octet)
 */
void SST26V_ReadStatusReg(SPI_t *spi, unsigned char* data);

/**
 * @brief Lit le registre de configuration.
 * @param spi Pointeur vers la configuration SPI
 * @param data Tampon pour stocker le registre de configuration (1 octet)
 */
void SST26V_ReadConfigurationReg(SPI_t *spi, unsigned char* data);

/**
 * @brief Lit l'ID JEDEC.
 * @param spi Pointeur vers la configuration SPI
 * @param data Tampon pour stocker l'ID JEDEC (3 octets)
 */
void SST26V_ReadJEDECIdReg(SPI_t *spi, unsigned char* data);

/**
 * @brief Lit les registres de protection de bloc.
 * @param spi Pointeur vers la configuration SPI
 * @param data Tampon pour stocker les registres de protection de bloc (18 octets)
 */
void SST26V_ReadBlockProtectionReg(SPI_t *spi, unsigned char* data);

#endif  // PLIB_MEMORY_H