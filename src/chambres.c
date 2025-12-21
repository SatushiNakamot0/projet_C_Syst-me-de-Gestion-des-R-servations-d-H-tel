#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chambres.h"
#include "structures.h"
#include "fichiers.h"
#include "../include/safe_input.h"
#include "../include/debug.h"

/* Constants dyal validation */
#define MIN_ROOM_NUMBER 1
#define MAX_ROOM_NUMBER 9999
#define MIN_PRICE 0.0f
#define MAX_PRICE 10000.0f

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

void ajouter_chambre(Chambre chambres[], int *count) {
    Chambre nouvelle_chambre;
    int numero_saisi;
    int numero_unique = 0;
    
    LOG_INFO("Début de l'ajout d'une nouvelle chambre");
    
    // N-checkiw ila kan 3endo l-space f l-array
    if (*count >= MAX_CHAMBRES) {
        LOG_ERROR("Limite de chambres atteinte (%d)", MAX_CHAMBRES);
        printf("\nErreur: Limite de chambres atteinte (%d). Ajout impossible.\n", MAX_CHAMBRES);
        return;
    }
    
    printf("\n--- AJOUT D'UNE NOUVELLE CHAMBRE ---\n");
    
    // Kan-demandiw l-numéro w kan-vérifiiw ila kan unique
    do {
        numero_saisi = safe_input_int("Entrez le numero de chambre (1-9999, doit etre unique): ");
        LOG_DEBUG("Numéro de chambre saisi: %d", numero_saisi);
        
        if (numero_saisi < MIN_ROOM_NUMBER || numero_saisi > MAX_ROOM_NUMBER) {
            printf("Erreur: Le numero doit etre entre %d et %d.\n", MIN_ROOM_NUMBER, MAX_ROOM_NUMBER);
            LOG_ERROR("Numéro hors limites: %d", numero_saisi);
            continue;
        }
        
        // Hna kan-checkiw ila l-numéro deja kayn
        if (chambre_numero_existe(chambres, *count, numero_saisi)) {
            printf("Erreur: Le numero %d existe deja. Veuillez choisir un autre numero.\n", numero_saisi);
            LOG_ERROR("Numéro de chambre dupliqué: %d", numero_saisi);
            numero_unique = 0;
        } else {
            numero_unique = 1;
            nouvelle_chambre.numero = numero_saisi;
            LOG_DEBUG("Numéro de chambre valide: %d", numero_saisi);
        }
    } while (!numero_unique);
    
    // Kan-demandiw l-type dyal l-chambre
    safe_input_string("Entrez le type de chambre (ex: Simple, Double, Suite): ", nouvelle_chambre.type, MAX_TYPE_LENGTH);
    LOG_DEBUG("Type de chambre: %s", nouvelle_chambre.type);
    
    // Kan-demandiw l-prix par nuit
    nouvelle_chambre.prix = safe_input_float("Entrez le prix par nuit (EUR): ");
    LOG_DEBUG("Prix saisi: %.2f", nouvelle_chambre.prix);
    
    if (nouvelle_chambre.prix < MIN_PRICE || nouvelle_chambre.prix > MAX_PRICE) {
        printf("Erreur: Prix invalide (doit etre entre %.0f et %.0f EUR).\n", MIN_PRICE, MAX_PRICE);
        LOG_ERROR("Prix hors limites: %.2f", nouvelle_chambre.prix);
        return;
    }
    
    // L-chambres jdad kaynin par défaut
    nouvelle_chambre.disponible = 1;
    
    // Validation finale dyal ga3 les données
    if (!valider_chambre(&nouvelle_chambre, chambres, *count, -1)) {
        printf("Erreur: Donnees de chambre invalides.\n");
        return;
    }
    
    // Kan-ajoutiw l-chambre l-array
    chambres[*count] = nouvelle_chambre;
    (*count)++;
    
    LOG_INFO("Chambre ajoutée avec succès: #%d %s (%.2f EUR)", 
             nouvelle_chambre.numero, nouvelle_chambre.type, nouvelle_chambre.prix);
    
    // Kan-sauvegardiw direct
    sauvegarder_chambres(chambres, *count);
    
    printf("Chambre %d (%s) ajoutee et sauvegardee avec succes.\n",
           nouvelle_chambre.numero, nouvelle_chambre.type);
}

void afficher_chambres(const Chambre chambres[], int count) {
    // N-checkiw ila kan 3endo chi chambre
    if (count == 0) {
        printf("\n--- AUCUNE CHAMBRE ENREGISTREE ---\n");
        return;
    }

    // Hna kan-affichiw l-header dyal l-tableau
    printf("\n======================================================================================\n");
    printf("  LISTE DES CHAMBRES ACTUELLES (%d)\n", count);
    printf("======================================================================================\n");
    printf("| Numero | Type | Prix/Nuit | Disponibilite\n");
    printf("--------------------------------------------------------------------------------------\n");

    // Kan-loopiw 3la ga3 les chambres w kan-affichiwhom
    for (int i = 0; i < count; i++) {
        printf("| %-7d | %-15s | %-9.2f | %-13s\n",
               chambres[i].numero,
               chambres[i].type,
               chambres[i].prix,
               chambres[i].disponible ? "Disponible" : "Occupee");
    }
    printf("======================================================================================\n");
}

void modifier_chambre(Chambre chambres[], int count) {
    int numero_a_modifier;
    int index_chambre = -1;
    
    LOG_INFO("Début de la modification d'une chambre");
    
    printf("\n--- MODIFICATION D'UNE CHAMBRE ---\n");
    numero_a_modifier = safe_input_int("Entrez le numero de la chambre a modifier: ");
    LOG_DEBUG("Numéro à modifier: %d", numero_a_modifier);
    
    // Kan-cherchiw l-chambre
    index_chambre = trouver_chambre_par_numero(chambres, count, numero_a_modifier);
    
    if (index_chambre == -1) {
        LOG_ERROR("Chambre non trouvée pour modification: %d", numero_a_modifier);
        printf("Erreur: Chambre numero %d non trouvee.\n", numero_a_modifier);
        return;
    }
    
    printf("\nChambre selectionnee: Numero %d | Type: %s | Prix: %.2f EUR | Disponible: %s\n",
           chambres[index_chambre].numero,
           chambres[index_chambre].type,
           chambres[index_chambre].prix,
           chambres[index_chambre].disponible ? "Oui" : "Non");
    
    int choix;
    printf("\nQue voulez-vous modifier ? (1: Type, 2: Prix, 3: Disponibilite): ");
    choix = safe_input_int("");
    
    // Kan-switchiw 3la l-choix dyal l-user
    switch (choix) {
        case 1: {
            // Modification dyal type
            char nouveau_type[MAX_TYPE_LENGTH];
            safe_input_string("Nouveau type (actuel: %s): ", nouveau_type, MAX_TYPE_LENGTH);
            strncpy(chambres[index_chambre].type, nouveau_type, sizeof(chambres[index_chambre].type) - 1);
            chambres[index_chambre].type[sizeof(chambres[index_chambre].type) - 1] = '\0';
            break;
        }
        
        case 2: {
            // Modification dyal prix
            float nouveau_prix;
            nouveau_prix = safe_input_float("Nouveau prix par nuit (actuel: %.2f EUR): ");
            if (nouveau_prix < MIN_PRICE || nouveau_prix > MAX_PRICE) {
                printf("Erreur: Prix invalide (doit etre entre %.0f et %.0f EUR).\n", MIN_PRICE, MAX_PRICE);
                return;
            }
            chambres[index_chambre].prix = nouveau_prix;
            break;
        }
        
        case 3: {
            // Modification dyal disponibilité
            int nouvelle_dispo;
            do {
                nouvelle_dispo = safe_input_int("Disponibilite (1=Disponible, 0=Non disponible, actuel: %d): ");
                if (nouvelle_dispo != 0 && nouvelle_dispo != 1) {
                    printf("Erreur: Valeur invalide (doit etre 0 ou 1).\n");
                }
            } while (nouvelle_dispo != 0 && nouvelle_dispo != 1);
            chambres[index_chambre].disponible = nouvelle_dispo;
            break;
        }
        
        default:
            LOG_ERROR("Choix invalide: %d", choix);
            printf("Choix invalide. Aucune modification effectuee.\n");
            return;
    }
    
    // N-validiw l-chambre moudifiée
    if (!valider_chambre(&chambres[index_chambre], chambres, count, index_chambre)) {
        LOG_ERROR("Données invalides après modification");
        printf("Erreur: Donnees invalides apres modification. Modification annulee.\n");
        return;
    }
    
    // Kan-sauvegardiw les modifications
    LOG_INFO("Chambre %d modifiée avec succès", numero_a_modifier);
    sauvegarder_chambres(chambres, count);
    printf("\nChambre %d modifiee et sauvegardee avec succes.\n", numero_a_modifier);
}

void supprimer_chambre(Chambre chambres[], int *count) {
    int numero_a_supprimer;
    int index_chambre = -1;
    
    LOG_INFO("Début de la suppression d'une chambre");
    
    printf("\n--- SUPPRESSION D'UNE CHAMBRE ---\n");
    numero_a_supprimer = safe_input_int("Entrez le numero de la chambre a supprimer: ");
    LOG_DEBUG("Numéro à supprimer: %d", numero_a_supprimer);
    
    // Kan-cherchiw l-chambre
    index_chambre = trouver_chambre_par_numero(chambres, *count, numero_a_supprimer);
    
    if (index_chambre == -1) {
        LOG_ERROR("Chambre non trouvée pour suppression: %d", numero_a_supprimer);
        printf("Erreur: Chambre numero %d non trouvee.\n", numero_a_supprimer);
        return;
    }
    
    // Kan-demandiw l-confirmation
    printf("\nConfirmez-vous la suppression de la chambre numero %d, Type: %s ? (o/n): ",
           chambres[index_chambre].numero, chambres[index_chambre].type);
    
    int confirmation = safe_input_yes_no("");
    
    if (!confirmation) {
        LOG_INFO("Suppression annulée par l'utilisateur: %d", numero_a_supprimer);
        printf("Suppression annulee.\n");
        return;
    }
    
    // Kan-supprimiw l-chambre mn l-array b shifting
    for (int i = index_chambre; i < (*count) - 1; i++) {
        chambres[i] = chambres[i + 1];
    }
    
    (*count)--;
    
    // Kan-sauvegardiw les modifications
    LOG_INFO("Chambre %d supprimée avec succès", numero_a_supprimer);
    sauvegarder_chambres(chambres, *count);
    printf("\nChambre %d supprimee et sauvegarde avec succes.\n", numero_a_supprimer);
}

void rechercher_chambre(const Chambre chambres[], int count) {
    char recherche[50];
    int trouve = 0;
    
    LOG_INFO("Début de la recherche de chambre");
    
    printf("\n--- RECHERCHE DE CHAMBRE ---\n");
    safe_input_string("Entrez le numero ou le type de chambre a rechercher: ", recherche, sizeof(recherche));
    LOG_DEBUG("Terme recherché: %s", recherche);
    
    printf("\nResultats pour '%s':\n", recherche);
    printf("------------------------------------------------------\n");
    
    // Kan-loopiw 3la ga3 les chambres w kan-recherchiw
    for (int i = 0; i < count; i++) {
        char numero_str[16];
        snprintf(numero_str, sizeof(numero_str), "%d", chambres[i].numero);
        
        // N-checkiw ila l-kalma kayna f numero ola type
        if (strstr(numero_str, recherche) != NULL ||
            strstr(chambres[i].type, recherche) != NULL) {
            
            printf("Numero: %d | Type: %s | Prix: %.2f EUR | Disponible: %s\n",
                   chambres[i].numero,
                   chambres[i].type,
                   chambres[i].prix,
                   chambres[i].disponible ? "Oui" : "Non");
            trouve = 1;
        }
    }
    
    if (!trouve) {
        printf("Aucune chambre trouvee pour '%s'.\n", recherche);
        LOG_INFO("Aucune chambre trouvée pour: %s", recherche);
    }
    printf("------------------------------------------------------\n");
}

