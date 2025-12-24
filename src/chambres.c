#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chambres.h"
#include "structures.h"
#include "fichiers.h"

#include "../include/debug.h"

/* Constants dyal validation */
#define MIN_ROOM_NUMBER 1
#define MAX_ROOM_NUMBER 9999
#define MIN_PRICE 0.0f
#define MAX_PRICE 10000.0f
#define MAX_TYPE_LENGTH 50  // Max length for room type string

/* ============================================================================
 * IMPLEMENTATION DYAL GESTION DES CHAMBRES
 * ============================================================================
 * Hna kan-geriw les chambres b daba, b validation w error handling
 * Kan-protegiw l-data mn l-corruption w kan-logiw l-errors
 * ============================================================================ */

int trouver_chambre_par_numero(const Chambre chambres[], int count, int numero) {
    // Kan-cherchiw l-chambre b numero w n-rajtiw l-index
    for (int i = 0; i < count; i++) {
        if (chambres[i].numero == numero) {
            return i; // L-chambre trouvée
        }
    }
    return -1; // L-chambre mazal machi kayna
}

int chambre_numero_existe(const Chambre chambres[], int count, int numero) {
    // N-checkiw ila l-chambre kayna deja
    return trouver_chambre_par_numero(chambres, count, numero) != -1;
}

int compter_chambres_disponibles(const Chambre chambres[], int count) {
    int disponibles = 0;
    // Kan-comptiw les chambres li kaynin
    for (int i = 0; i < count; i++) {
        if (chambres[i].disponible == 1) {
            disponibles++;
        }
    }
    return disponibles;
}

int valider_chambre(const Chambre *chambre, const Chambre chambres[], int count, int exclude_index) {
    if (!chambre) {
        return 0; // Pointeur invalide
    }
    
    // N-validiw l-numéro dyal l-chambre
    if (chambre->numero < MIN_ROOM_NUMBER || chambre->numero > MAX_ROOM_NUMBER) {
        LOG_ERROR("Numéro de chambre invalide: %d (doit être %d-%d)", 
                  chambre->numero, MIN_ROOM_NUMBER, MAX_ROOM_NUMBER);
        return 0; // L-numéro hors limites
    }
    
    // N-checkiw ila l-numéro unique (n-stexnaw l-index dyal modification)
    for (int i = 0; i < count; i++) {
        if (i != exclude_index && chambres[i].numero == chambre->numero) {
            return 0; // L-numéro deja kayn
        }
    }
    
    // N-validiw l-type dyal l-chambre
    if (strlen(chambre->type) == 0 || strlen(chambre->type) >= MAX_TYPE_LENGTH) {
        LOG_ERROR("Type de chambre invalide: '%s' (longueur: %zu)", chambre->type, strlen(chambre->type));
        return 0; // Type invalide
    }
    
    // N-validiw l-prix
    if (chambre->prix < MIN_PRICE || chambre->prix > MAX_PRICE) {
        LOG_ERROR("Prix de chambre invalide: %.2f (doit être %.0f-%.0f)", 
                  chambre->prix, MIN_PRICE, MAX_PRICE);
        return 0; // Prix hors limites
    }
    
    // N-validiw l-statut dyal disponibilité
    if (chambre->disponible != 0 && chambre->disponible != 1) {
        LOG_ERROR("Statut de disponibilité invalide: %d", chambre->disponible);
        return 0; // Statut invalide
    }
    
    LOG_DEBUG("Validation de chambre réussie: #%d %s", chambre->numero, chambre->type);
    return 1; // L-chambre valide
}

/* ============================================================================
 * CRUD OPERATIONS (PURE LOGIC)
 * ============================================================================ */

int chambre_ajouter(Chambre *chambres, int *count, const Chambre *nouvelle_chambre) {
    if (*count >= MAX_CHAMBRES) {
        LOG_ERROR("Erreur: Limite de chambres atteinte (%d)", MAX_CHAMBRES);
        return -1; // Full
    }
    
    // Validate the new room
    if (!valider_chambre(nouvelle_chambre, chambres, *count, -1)) {
        return -2; // Invalid data
    }

    // Add to array
    chambres[*count] = *nouvelle_chambre;
    (*count)++;

    // Persist
    sauvegarder_chambres(chambres, *count);
    LOG_INFO("Chambre ajoutée: #%d %s", nouvelle_chambre->numero, nouvelle_chambre->type);
    return 0; // Success
}

int chambre_modifier(Chambre *chambres, int count, const Chambre *modifiee) {
    int index = trouver_chambre_par_numero(chambres, count, modifiee->numero);
    if (index == -1) {
        return -1; // Not found
    }

    // Validate using the index to exclude itself from uniqueness checks if number changed (which it shouldn't here, but good practice)
    // Note: Usually primary key (numero) shouldn't change in update. Assuming 'modifiee' has the same 'numero'.
    if (!valider_chambre(modifiee, chambres, count, index)) {
        return -2; // Invalid data
    }

    // Update
    chambres[index] = *modifiee;

    // Persist
    sauvegarder_chambres(chambres, count);
    LOG_INFO("Chambre modifiée: #%d", modifiee->numero);
    return 0;
}

int chambre_supprimer(Chambre *chambres, int *count, int numero) {
    int index = trouver_chambre_par_numero(chambres, *count, numero);
    if (index == -1) {
        return -1; // Not found
    }

    // Shift remaining
    for (int i = index; i < (*count) - 1; i++) {
        chambres[i] = chambres[i + 1];
    }
    (*count)--;

    // Persist
    sauvegarder_chambres(chambres, *count);
    LOG_INFO("Chambre supprimée: #%d", numero);
    return 0;
}

