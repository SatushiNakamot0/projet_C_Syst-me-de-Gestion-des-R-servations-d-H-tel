#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chambres.h"
#include "structures.h"
#include "fichiers.h"

#include "../include/debug.h"

/* Constants bach nvalidiw les chambres */
#define MIN_ROOM_NUMBER 1
#define MAX_ROOM_NUMBER 9999
#define MIN_PRICE 0.0f
#define MAX_PRICE 10000.0f
#define MAX_TYPE_LENGTH 50  // T9ad d-type dyal chambre

/* ============================================================================
 * GESTION DYAL LES CHAMBRES
 * ============================================================================
 * Hna kandirou gestion dyal les chambres b validation w error handling
 * Kanprotégiw l-data mn l-corruption w kanlogiw l-errors
 * ============================================================================ */

int trouver_chambre_par_numero(const Chambre chambres[], int count, int numero) {
    // Kan9lbou 3la chambre b numero dyalha
    for (int i = 0; i < count; i++) {
        if (chambres[i].numero == numero) {
            return i; // L9inaha
        }
    }
    return -1; // Ma kaynach
}

int chambre_numero_existe(const Chambre chambres[], int count, int numero) {
    // Nchoufou wach l-numero dyal chambre deja kayn
    return trouver_chambre_par_numero(chambres, count, numero) != -1;
}

int compter_chambres_disponibles(const Chambre chambres[], int count) {
    int disponibles = 0;
    // Ncomptiw chal mn chambre khawya
    for (int i = 0; i < count; i++) {
        if (chambres[i].disponible == 1) {
            disponibles++;
        }
    }
    return disponibles;
}

int valider_chambre(const Chambre *chambre, const Chambre chambres[], int count, int exclude_index) {
    if (!chambre) {
        return 0; // Pointer khawi
    }
    
    // Nvalidiw n-numero dyal chambre
    if (chambre->numero < MIN_ROOM_NUMBER || chambre->numero > MAX_ROOM_NUMBER) {
        LOG_ERROR("Numéro de chambre invalide: %d (doit être %d-%d)", 
                  chambre->numero, MIN_ROOM_NUMBER, MAX_ROOM_NUMBER);
        return 0; // Numero kharj mn l-7doud
    }
    
    // Nchoufou wach numero unique (n-stathniw index dyal modification)
    for (int i = 0; i < count; i++) {
        if (i != exclude_index && chambres[i].numero == chambre->numero) {
            return 0; // Numero deja kayn
        }
    }
    
    // Nvalidiw type dyal chambre
    if (strlen(chambre->type) == 0 || strlen(chambre->type) >= MAX_TYPE_LENGTH) {
        LOG_ERROR("Type de chambre invalide: '%s' (longueur: %zu)", chambre->type, strlen(chambre->type));
        return 0; // Type ma sali7ch
    }
    
    // Nvalidiw price
    if (chambre->prix < MIN_PRICE || chambre->prix > MAX_PRICE) {
        LOG_ERROR("Prix de chambre invalide: %.2f (doit être %.0f-%.0f)", 
                  chambre->prix, MIN_PRICE, MAX_PRICE);
        return 0; // Prix kharj mn l-7doud
    }
    
    // Nvalidiw statut dyal disponibilité
    if (chambre->disponible != 0 && chambre->disponible != 1) {
        LOG_ERROR("Statut de disponibilité invalide: %d", chambre->disponible);
        return 0; // Statut ma sali7ch
    }
    
    LOG_DEBUG("Validation de chambre réussie: #%d %s", chambre->numero, chambre->type);
    return 1; // Chambre mezyana
}

/* ============================================================================
 * OPÉRATIONS CRUD (LOGIC PURE)
 * ============================================================================ */

int chambre_ajouter(Chambre *chambres, int *count, const Chambre *nouvelle_chambre) {
    if (*count >= MAX_CHAMBRES) {
        LOG_ERROR("Erreur: Limite de chambres atteinte (%d)", MAX_CHAMBRES);
        return -1; // 3amer, ma9drnach nzidou
    }
    
    // Nvalidiw chambre jdida
    if (!valider_chambre(nouvelle_chambre, chambres, *count, -1)) {
        return -2; // Data ma sali7ach
    }

    // Nziwoha l-array
    chambres[*count] = *nouvelle_chambre;
    (*count)++;

    // Nsauvgardiw
    sauvegarder_chambres(chambres, *count);
    LOG_INFO("Chambre ajoutée: #%d %s", nouvelle_chambre->numero, nouvelle_chambre->type);
    return 0; // Kamlet b njah
}

int chambre_modifier(Chambre *chambres, int count, const Chambre *modifiee) {
    int index = trouver_chambre_par_numero(chambres, count, modifiee->numero);
    if (index == -1) {
        return -1; // Ma l9inahach
    }

    // Nvalidiw l-modification (nstathniw nafs index bash ma ydirch conflict)
    if (!valider_chambre(modifiee, chambres, count, index)) {
        return -2; // Data ma sali7ach
    }

    // Nbedlou
    chambres[index] = *modifiee;

    // Nsauvgardiw
    sauvegarder_chambres(chambres, count);
    LOG_INFO("Chambre modifiée: #%d", modifiee->numero);
    return 0;
}

int chambre_supprimer(Chambre *chambres, int *count, int numero) {
    int index = trouver_chambre_par_numero(chambres, *count, numero);
    if (index == -1) {
        return -1; // Ma l9inahach
    }

    // N7yydo w n9admo li ba3do
    for (int i = index; i < (*count) - 1; i++) {
        chambres[i] = chambres[i + 1];
    }
    (*count)--;

    // Nsauvgardiw
    sauvegarder_chambres(chambres, *count);
    LOG_INFO("Chambre supprimée: #%d", numero);
    return 0;
}
