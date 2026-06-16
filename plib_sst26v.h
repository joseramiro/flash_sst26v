/**
 * @file plib_sst26v.h
 * @author Ramiro Najera
 * @brief Fichier en-tête principal pour la mémoire flash SST26V
 * 
 * Ce fichier en-tête est le point d'entrée principal pour la bibliothèque de gestion des la mémoire
 * flash SST26V. Il inclut les autres fichiers d'en-tête nécessaires pour la gestion de
 * communication SPI avec la mémoire flash SST26V et le contrôle du Chip Select (CS).
 * @version 1.0.4
 * @date 2025-04-23
 * @copyright Copyright (c) 2025
 */

#ifndef PLIB_MEMORY_H
#define PLIB_MEMORY_H

#include <stdint.h>
#include "libs/common_c_libs/plib_comm_struct.h"

/**
 * @brief Liste de registres pour communication SPI
 */
typedef enum
{
    SST26V_NOP = 0x00,      /**< No Operation: annule uniquement une commande de Réinitialisation Active (RSTEN) */
    SST26V_RSTEN = 0x66,    /**< Réinitialisation Active: Réinitialisation système (logicielle) */
    SST26V_RST = 0x99,      /**< Réinitialisation: Réinitialisation après SST26V_RSTEN */
    SST26V_RDSR = 0x05,     /**< Envoie le contenu du registre STATUS */
    SST26V_WRSR = 0x01,     /**< Ecrit de nouvelles valeurs dans le registre de configuration */
    SST26V_RDCR = 0x35,     /**< Envoient le contenu des registres d'état et de configuration */
    SST26V_READ = 0x03,     /**< Lecture de la flash */
    SST26V_SB = 0xC0,       /**< Spécifie le nombre d'octets à envoyer lors d'une commande de lecture */
    SST26V_JEDEC_ID = 0x9F, /**< Lit le JEDEC-ID identifie le dispositif comme étant le SST26VF032B/032BA */
    SST26V_WREN = 0x06,     /**< Met le bit WriteEnable-Latch du registre d'état à '1' */
    SST26V_WRDI = 0x04,     /**< Met le bit WriteEnable-Latch du registre d'état à '0' */
    SST26V_SE = 0x20,       /**< Efface tous les bits du secteur de 4 Ko sélectionné à '1' */
    SST26V_BE = 0xD8,       /**< Efface tous les bits du bloc sélectionné à '1' */
    SST26V_CE = 0xC7,       /**< Efface tous les bits du dispositif à '1' */
    SST26V_PP = 0x02,       /**< Programme jusqu'à 256 octets de données dans la mémoire */
    SST26V_WRSU = 0xB0,     /**< Permet à l'hôte de programmer ou de lire tout secteur qui n'était pas en cours d'effacement. */
    SST26V_WRRE = 0x30,     /**< L'instruction Write-Resume redémarre une commande d'écriture */
    SST26V_RBPR = 0x72,     /**< L'instruction de lecture du registre de protection de bloc */
    SST26V_WBPR = 0x42      /**< Modifie les données du registre de protection de bloc */
}SST26VRegisters_t;

/** 
 * @struct SST26V_t
 * @brief Structure de configuration de mémoire flash SST26V 
 */
typedef struct
{
    uint8_t id;     /**< Identifiant de l'objet */
    SPI_t spi;      /**< Configuration SPI */
}SST26V_t;

/**
 * @brief Initialise le module SST26V avec la configuration fournie
 * @param obj Pointeur vers la configuration du module
 * @return uint8_t 0 ok, sinon erreur
 */
uint8_t SST26V_Init(SST26V_t *obj);

/**
 * @brief Lit des données de la mémoire flash
 * @param obj Pointeur vers la configuration du module
 * @param address Adresse du début de la lecture
 * @param size Nombre de bytes à lire
 * @param data Données lues
 */
void SST26V_ReadMemory(SST26V_t *obj, uint32_t address, uint16_t size, uint8_t* data);

/**
 * @brief Ecrit des données dans la mémoire flash
 * @param obj Pointeur vers la configuration du module
 * @param data Données à écrire
 * @param address Adresse du début de l'écriture
 * @param size Nombre de bytes à lire
 * @return uint8_t 0 ok, sinon erreur
 */
uint8_t SST26V_WriteMemory(SST26V_t *obj, uint8_t* data, uint32_t address, uint16_t size);

/**
 * @brief Efface une page dans la mémoire flash
 * @param obj Pointeur vers la configuration du module
 * @param address Adresse dont la page va être supprimée
 * @return uint8_t 0 ok, sinon erreur
 */
uint8_t SST26V_EraseSector(SST26V_t *obj, uint32_t address);

/**
 * @brief Efface toutes les pages dans la mémoire flash
 * @param obj Pointeur vers la configuration du module
 * @return uint8_t 0 ok, sinon erreur
 */
uint8_t SST26V_EraseChip(SST26V_t *obj);

#endif  // PLIB_MEMORY_H