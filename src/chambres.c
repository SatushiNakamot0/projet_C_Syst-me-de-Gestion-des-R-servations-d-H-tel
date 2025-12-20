#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chambres.h"
#include "structures.h"
#include "fichiers.h"

/* ============================================================================
 * ROOM MANAGEMENT IMPLEMENTATION
 * ============================================================================
 * Enterprise-grade room CRUD with validation, error handling, and persistence.
 * Maintains reference quality standards: clear errors, no silent failures.
 * ============================================================================ */

int trouver_chambre_par_numero(const Chambre chambres[], int count, int numero) {
    for (int i = 0; i < count; i++) {
        if (chambres[i].numero == numero) {
            return i;
        }
    }
    return -1;
}

int chambre_numero_existe(const Chambre chambres[], int count, int numero) {
    return trouver_chambre_par_numero(chambres, count, numero) != -1;
}

int compter_chambres_disponibles(const Chambre chambres[], int count) {
    int disponibles = 0;
    for (int i = 0; i < count; i++) {
        if (chambres[i].disponible == 1) {
            disponibles++;
        }
    }
    return disponibles;
}

int valider_chambre(const Chambre *chambre, const Chambre chambres[], int count, int exclude_index) {
    if (!chambre) {
        return 0; /* Invalid pointer */
    }
    
    /* Validate room number */
    if (chambre->numero <= 0 || chambre->numero > 9999) {
        return 0; /* Invalid room number range */
    }
    
    /* Check uniqueness (excluding current index for modifications) */
    for (int i = 0; i < count; i++) {
        if (i != exclude_index && chambres[i].numero == chambre->numero) {
            return 0; /* Room number already exists */
        }
    }
    
    /* Validate type */
    if (strlen(chambre->type) == 0 || strlen(chambre->type) > 19) {
        return 0; /* Invalid type length */
    }
    
    /* Validate price */
    if (chambre->prix < 0.0f || chambre->prix > 10000.0f) {
        return 0; /* Invalid price range */
    }
    
    /* Validate availability status */
    if (chambre->disponible != 0 && chambre->disponible != 1) {
        return 0; /* Invalid availability status */
    }
    
    return 1; /* Valid */
}

void ajouter_chambre(Chambre chambres[], int *count) {
    Chambre nouvelle_chambre;
    int numero_saisi;
    int numero_unique = 0;
    
    if (*count >= MAX_CHAMBRES) {
        printf("\nErreur: Limite de chambres atteinte (%d). Ajout impossible.\n", MAX_CHAMBRES);
        return;
    }
    
    printf("\n--- AJOUT D'UNE NOUVELLE CHAMBRE ---\n");
    
    /* Validate and get unique room number */
    do {
        printf("Entrez le numero de chambre (1-9999, doit etre unique): ");
        if (scanf("%d", &numero_saisi) != 1) {
            printf("Erreur: Numero invalide.\n");
            return;
        }
        
        if (numero_saisi <= 0 || numero_saisi > 9999) {
            printf("Erreur: Le numero doit etre entre 1 et 9999.\n");
            continue;
        }
        
        if (chambre_numero_existe(chambres, *count, numero_saisi)) {
            printf("Erreur: Le numero %d existe deja. Veuillez choisir un autre numero.\n", numero_saisi);
            numero_unique = 0;
        } else {
            numero_unique = 1;
            nouvelle_chambre.numero = numero_saisi;
        }
    } while (!numero_unique);
    
    /* Get room type */
    printf("Entrez le type de chambre (ex: Simple, Double, Suite): ");
    if (scanf("%19s", nouvelle_chambre.type) != 1) {
        printf("Erreur: Type invalide.\n");
        return;
    }
    
    /* Get price per night */
    printf("Entrez le prix par nuit (EUR): ");
    if (scanf("%f", &nouvelle_chambre.prix) != 1) {
        printf("Erreur: Prix invalide.\n");
        return;
    }
    
    if (nouvelle_chambre.prix < 0.0f || nouvelle_chambre.prix > 10000.0f) {
        printf("Erreur: Prix invalide (doit etre entre 0 et 10000 EUR).\n");
        return;
    }
    
    /* Set availability - new rooms are available by default */
    nouvelle_chambre.disponible = 1;
    
    /* Final validation */
    if (!valider_chambre(&nouvelle_chambre, chambres, *count, -1)) {
        printf("Erreur: Donnees de chambre invalides.\n");
        return;
    }
    
    /* Add to array */
    chambres[*count] = nouvelle_chambre;
    (*count)++;
    
    /* Persist immediately */
    sauvegarder_chambres(chambres, *count);
    
    printf("Chambre %d (%s) ajoutee et sauvegardee avec succes.\n",
           nouvelle_chambre.numero, nouvelle_chambre.type);
}

void afficher_chambres(const Chambre chambres[], int count) {
    if (count == 0) {
        printf("\n--- AUCUNE CHAMBRE ENREGISTREE ---\n");
        return;
    }
    
    printf("\n======================================================================================\n");
    printf("  LISTE DES CHAMBRES (%d)\n", count);
    printf("======================================================================================\n");
    printf("| Numero | Type      | Prix/Nuit  | Disponible\n");
    printf("--------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        const char *dispo_str = chambres[i].disponible ? "Oui" : "Non";
        printf("| %-7d | %-9s | %-10.2f | %s\n",
               chambres[i].numero,
               chambres[i].type,
               chambres[i].prix,
               dispo_str);
    }
    
    int disponibles = compter_chambres_disponibles(chambres, count);
    printf("--------------------------------------------------------------------------------------\n");
    printf("Total: %d chambres | Disponibles: %d | Occupees: %d\n",
           count, disponibles, count - disponibles);
    printf("======================================================================================\n");
}

void modifier_chambre(Chambre chambres[], int count) {
    int numero_a_modifier;
    int index_chambre = -1;
    
    printf("\n--- MODIFICATION D'UNE CHAMBRE ---\n");
    printf("Entrez le numero de la chambre a modifier: ");
    if (scanf("%d", &numero_a_modifier) != 1) {
        printf("Erreur: Numero invalide.\n");
        return;
    }
    
    /* Find room */
    index_chambre = trouver_chambre_par_numero(chambres, count, numero_a_modifier);
    
    if (index_chambre == -1) {
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
    if (scanf("%d", &choix) != 1) {
        printf("Erreur: Choix invalide.\n");
        return;
    }
    
    switch (choix) {
        case 1: {
            char nouveau_type[20];
            printf("Nouveau type (actuel: %s): ", chambres[index_chambre].type);
            if (scanf("%19s", nouveau_type) != 1) {
                printf("Erreur: Type invalide.\n");
                return;
            }
            strncpy(chambres[index_chambre].type, nouveau_type, sizeof(chambres[index_chambre].type) - 1);
            chambres[index_chambre].type[sizeof(chambres[index_chambre].type) - 1] = '\0';
            break;
        }
        
        case 2: {
            float nouveau_prix;
            printf("Nouveau prix par nuit (actuel: %.2f EUR): ", chambres[index_chambre].prix);
            if (scanf("%f", &nouveau_prix) != 1) {
                printf("Erreur: Prix invalide.\n");
                return;
            }
            if (nouveau_prix < 0.0f || nouveau_prix > 10000.0f) {
                printf("Erreur: Prix invalide (doit etre entre 0 et 10000 EUR).\n");
                return;
            }
            chambres[index_chambre].prix = nouveau_prix;
            break;
        }
        
        case 3: {
            int nouvelle_dispo;
            printf("Disponibilite (1=Disponible, 0=Non disponible, actuel: %d): ",
                   chambres[index_chambre].disponible);
            if (scanf("%d", &nouvelle_dispo) != 1) {
                printf("Erreur: Valeur invalide.\n");
                return;
            }
            if (nouvelle_dispo != 0 && nouvelle_dispo != 1) {
                printf("Erreur: Valeur invalide (doit etre 0 ou 1).\n");
                return;
            }
            chambres[index_chambre].disponible = nouvelle_dispo;
            break;
        }
        
        default:
            printf("Choix invalide. Aucune modification effectuee.\n");
            return;
    }
    
    /* Validate modified room */
    if (!valider_chambre(&chambres[index_chambre], chambres, count, index_chambre)) {
        printf("Erreur: Donnees invalides apres modification. Modification annulee.\n");
        return;
    }
    
    /* Persist changes */
    sauvegarder_chambres(chambres, count);
    printf("\nChambre %d modifiee et sauvegardee avec succes.\n", numero_a_modifier);
}

void supprimer_chambre(Chambre chambres[], int *count) {
    int numero_a_supprimer;
    int index_chambre = -1;
    
    printf("\n--- SUPPRESSION D'UNE CHAMBRE ---\n");
    printf("Entrez le numero de la chambre a supprimer: ");
    if (scanf("%d", &numero_a_supprimer) != 1) {
        printf("Erreur: Numero invalide.\n");
        return;
    }
    
    /* Find room */
    index_chambre = trouver_chambre_par_numero(chambres, *count, numero_a_supprimer);
    
    if (index_chambre == -1) {
        printf("Erreur: Chambre numero %d non trouvee.\n", numero_a_supprimer);
        return;
    }
    
    /* Confirmation */
    printf("\nConfirmez-vous la suppression de la chambre numero %d, Type: %s ? (o/n): ",
           chambres[index_chambre].numero, chambres[index_chambre].type);
    char confirmation;
    scanf(" %c", &confirmation);
    
    if (confirmation != 'o' && confirmation != 'O') {
        printf("Suppression annulee.\n");
        return;
    }
    
    /* Remove from array by shifting */
    for (int i = index_chambre; i < (*count) - 1; i++) {
        chambres[i] = chambres[i + 1];
    }
    
    (*count)--;
    
    /* Persist changes */
    sauvegarder_chambres(chambres, *count);
    printf("\nChambre %d supprimee et sauvegarde avec succes.\n", numero_a_supprimer);
}

void rechercher_chambre(const Chambre chambres[], int count) {
    char recherche[50];
    int trouve = 0;
    
    printf("\n--- RECHERCHE DE CHAMBRE ---\n");
    printf("Entrez le numero ou le type de chambre a rechercher: ");
    if (scanf("%49s", recherche) != 1) {
        printf("Erreur: Recherche invalide.\n");
        return;
    }
    
    printf("\nResultats pour '%s':\n", recherche);
    printf("------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        char numero_str[16];
        snprintf(numero_str, sizeof(numero_str), "%d", chambres[i].numero);
        
        /* Search by number or type */
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
    }
    printf("------------------------------------------------------\n");
}

