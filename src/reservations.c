#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reservations.h"
#include "structures.h"
#include "fichiers.h"
#include "clients.h"
#include "chambres.h"

/* ============================================================================
 * RESERVATION MANAGEMENT IMPLEMENTATION
 * ============================================================================
 * Enterprise-grade reservation engine with conflict detection, date validation,
 * and automatic calculations. Prevents double-booking and data corruption.
 * ============================================================================ */

/* Helper: Extract day, month, year from date string DD/MM/YYYY */
static void parser_date(const char *date, int *jour, int *mois, int *annee) {
    if (!date || !jour || !mois || !annee) return;
    
    *jour = 0;
    *mois = 0;
    *annee = 0;
    
    if (strlen(date) != 10 || date[2] != '/' || date[5] != '/') {
        return;
    }
    
    *jour = atoi(date);
    *mois = atoi(date + 3);
    *annee = atoi(date + 6);
}

int valider_date(const char *date) {
    if (!date || strlen(date) != 10) {
        return 0;
    }
    
    if (date[2] != '/' || date[5] != '/') {
        return 0;
    }
    
    int jour, mois, annee;
    parser_date(date, &jour, &mois, &annee);
    
    if (jour < 1 || jour > 31) return 0;
    if (mois < 1 || mois > 12) return 0;
    if (annee < 2020 || annee > 2100) return 0;
    
    /* Basic month day validation */
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (jour > jours_par_mois[mois - 1]) {
        return 0;
    }
    
    return 1;
}

int comparer_dates(const char *date1, const char *date2) {
    if (!date1 || !date2) return 0;
    
    int j1, m1, a1, j2, m2, a2;
    parser_date(date1, &j1, &m1, &a1);
    parser_date(date2, &j2, &m2, &a2);
    
    if (a1 < a2) return -1;
    if (a1 > a2) return 1;
    if (m1 < m2) return -1;
    if (m1 > m2) return 1;
    if (j1 < j2) return -1;
    if (j1 > j2) return 1;
    return 0;
}

int calculer_nuits(const char *date_debut, const char *date_fin) {
    if (!valider_date(date_debut) || !valider_date(date_fin)) {
        return -1;
    }
    
    int j1, m1, a1, j2, m2, a2;
    parser_date(date_debut, &j1, &m1, &a1);
    parser_date(date_fin, &j2, &m2, &a2);
    
    /* Simple calculation: convert to days since epoch (simplified) */
    /* For production, use proper date library, but this works for basic cases */
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    long jours1 = a1 * 365 + j1;
    for (int i = 0; i < m1 - 1; i++) {
        jours1 += jours_par_mois[i];
    }
    
    long jours2 = a2 * 365 + j2;
    for (int i = 0; i < m2 - 1; i++) {
        jours2 += jours_par_mois[i];
    }
    
    int nuits = (int)(jours2 - jours1);
    return nuits > 0 ? nuits : 0;
}

float calculer_montant_reservation(int nuits, float prix_nuit) {
    if (nuits < 0 || prix_nuit < 0) {
        return 0.0f;
    }
    return nuits * prix_nuit;
}

int chambre_disponible_dates(const Reservation reservations[], int count,
                             int chambre_numero,
                             const char *date_debut, const char *date_fin,
                             int exclude_reservation_id) {
    if (!valider_date(date_debut) || !valider_date(date_fin)) {
        return 0; /* Invalid dates */
    }
    
    if (comparer_dates(date_debut, date_fin) >= 0) {
        return 0; /* Start date must be before end date */
    }
    
    /* Check all reservations for this room */
    for (int i = 0; i < count; i++) {
        /* Skip the reservation we're modifying */
        if (reservations[i].id == exclude_reservation_id) {
            continue;
        }
        
        /* Check if same room */
        if (reservations[i].chambre_numero != chambre_numero) {
            continue;
        }
        
        /* Check for date overlap */
        /* Overlap occurs if: (start1 < end2) && (start2 < end1) */
        if (comparer_dates(date_debut, reservations[i].date_fin) < 0 &&
            comparer_dates(reservations[i].date_debut, date_fin) < 0) {
            return 0; /* Conflict found */
        }
    }
    
    return 1; /* Available */
}

int trouver_reservation_par_id(const Reservation reservations[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (reservations[i].id == id) {
            return i;
        }
    }
    return -1;
}

const Reservation* obtenir_reservation(const Reservation reservations[], int count, int index) {
    if (index < 0 || index >= count) {
        return NULL;
    }
    return &reservations[index];
}

void ajouter_reservation(Reservation reservations[], int *count,
                        const Client clients[], int clients_count,
                        const Chambre chambres[], int chambres_count) {
    Reservation nouvelle_reservation;
    int client_id, chambre_numero;
    int client_trouve = 0, chambre_trouve = 0;
    int max_id = 0;
    char date_debut[11], date_fin[11];
    
    if (*count >= MAX_RESERVATIONS) {
        printf("\nErreur: Limite de reservations atteinte (%d). Ajout impossible.\n", MAX_RESERVATIONS);
        return;
    }
    
    if (clients_count == 0) {
        printf("\nErreur: Aucun client enregistre. Veuillez d'abord ajouter un client.\n");
        return;
    }
    
    if (chambres_count == 0) {
        printf("\nErreur: Aucune chambre enregistree. Veuillez d'abord ajouter des chambres.\n");
        return;
    }
    
    printf("\n--- CREATION D'UNE RESERVATION ---\n");
    
    /* Get and validate client ID */
    printf("Entrez l'ID du client: ");
    if (scanf("%d", &client_id) != 1) {
        printf("Erreur: ID client invalide.\n");
        return;
    }
    
    for (int i = 0; i < clients_count; i++) {
        if (clients[i].id == client_id) {
            client_trouve = 1;
            printf("Client trouve: %s %s\n", clients[i].prenom, clients[i].nom);
            break;
        }
    }
    
    if (!client_trouve) {
        printf("Erreur: Client avec ID %d non trouve.\n", client_id);
        return;
    }
    
    /* Get and validate room number */
    printf("Entrez le numero de chambre: ");
    if (scanf("%d", &chambre_numero) != 1) {
        printf("Erreur: Numero de chambre invalide.\n");
        return;
    }
    
    int index_chambre = trouver_chambre_par_numero(chambres, chambres_count, chambre_numero);
    if (index_chambre == -1) {
        printf("Erreur: Chambre numero %d non trouvee.\n", chambre_numero);
        return;
    }
    
    if (chambres[index_chambre].disponible == 0) {
        printf("Erreur: Chambre %d n'est pas disponible.\n", chambre_numero);
        return;
    }
    
    printf("Chambre trouvee: %s - %.2f EUR/nuit\n",
           chambres[index_chambre].type, chambres[index_chambre].prix);
    
    /* Get and validate dates */
    printf("Entrez la date de debut (DD/MM/YYYY): ");
    if (scanf("%10s", date_debut) != 1) {
        printf("Erreur: Date invalide.\n");
        return;
    }
    
    if (!valider_date(date_debut)) {
        printf("Erreur: Format de date invalide. Utilisez DD/MM/YYYY.\n");
        return;
    }
    
    printf("Entrez la date de fin (DD/MM/YYYY): ");
    if (scanf("%10s", date_fin) != 1) {
        printf("Erreur: Date invalide.\n");
        return;
    }
    
    if (!valider_date(date_fin)) {
        printf("Erreur: Format de date invalide. Utilisez DD/MM/YYYY.\n");
        return;
    }
    
    if (comparer_dates(date_debut, date_fin) >= 0) {
        printf("Erreur: La date de fin doit etre apres la date de debut.\n");
        return;
    }
    
    /* Check room availability for these dates */
    if (!chambre_disponible_dates(reservations, *count, chambre_numero,
                                  date_debut, date_fin, -1)) {
        printf("Erreur: La chambre %d n'est pas disponible pour ces dates.\n", chambre_numero);
        printf("        Il existe deja une reservation qui chevauche cette periode.\n");
        return;
    }
    
    /* Calculate nights and amount */
    int nuits = calculer_nuits(date_debut, date_fin);
    if (nuits <= 0) {
        printf("Erreur: Calcul du nombre de nuits invalide.\n");
        return;
    }
    
    float montant = calculer_montant_reservation(nuits, chambres[index_chambre].prix);
    
    printf("\nResume de la reservation:\n");
    printf("  Client ID: %d\n", client_id);
    printf("  Chambre: %d (%s)\n", chambre_numero, chambres[index_chambre].type);
    printf("  Dates: %s -> %s (%d nuits)\n", date_debut, date_fin, nuits);
    printf("  Montant total: %.2f EUR\n", montant);
    
    printf("\nConfirmez-vous cette reservation ? (o/n): ");
    char confirmation;
    scanf(" %c", &confirmation);
    
    if (confirmation != 'o' && confirmation != 'O') {
        printf("Reservation annulee.\n");
        return;
    }
    
    /* Generate ID */
    for (int i = 0; i < *count; i++) {
        if (reservations[i].id > max_id) {
            max_id = reservations[i].id;
        }
    }
    nouvelle_reservation.id = max_id + 1;
    
    /* Fill reservation data */
    nouvelle_reservation.client_id = client_id;
    nouvelle_reservation.chambre_numero = chambre_numero;
    strncpy(nouvelle_reservation.date_debut, date_debut, sizeof(nouvelle_reservation.date_debut) - 1);
    nouvelle_reservation.date_debut[sizeof(nouvelle_reservation.date_debut) - 1] = '\0';
    strncpy(nouvelle_reservation.date_fin, date_fin, sizeof(nouvelle_reservation.date_fin) - 1);
    nouvelle_reservation.date_fin[sizeof(nouvelle_reservation.date_fin) - 1] = '\0';
    nouvelle_reservation.montant = montant;
    
    /* Add to array */
    reservations[*count] = nouvelle_reservation;
    (*count)++;
    
    /* Persist immediately */
    sauvegarder_reservations(reservations, *count);
    
    printf("\nReservation creee avec succes (ID: %d)\n", nouvelle_reservation.id);
    printf("Montant total: %.2f EUR pour %d nuits.\n", montant, nuits);
}

void afficher_reservations(const Reservation reservations[], int count,
                          const Client clients[], int clients_count,
                          const Chambre chambres[], int chambres_count) {
    if (count == 0) {
        printf("\n--- AUCUNE RESERVATION ENREGISTREE ---\n");
        return;
    }
    
    printf("\n======================================================================================\n");
    printf("  LISTE DES RESERVATIONS (%d)\n", count);
    printf("======================================================================================\n");
    printf("| ID | Client      | Chambre | Date Debut | Date Fin   | Montant\n");
    printf("--------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        /* Find client name */
        const char *nom_client = "Inconnu";
        const char *prenom_client = "";
        for (int j = 0; j < clients_count; j++) {
            if (clients[j].id == reservations[i].client_id) {
                nom_client = clients[j].nom;
                prenom_client = clients[j].prenom;
                break;
            }
        }
        
        /* Find room type */
        const char *type_chambre = "Inconnu";
        for (int j = 0; j < chambres_count; j++) {
            if (chambres[j].numero == reservations[i].chambre_numero) {
                type_chambre = chambres[j].type;
                break;
            }
        }
        
        printf("| %-4d | %-11s | %-7d | %-10s | %-10s | %.2f EUR\n",
               reservations[i].id,
               prenom_client,
               reservations[i].chambre_numero,
               reservations[i].date_debut,
               reservations[i].date_fin,
               reservations[i].montant);
    }
    
    printf("======================================================================================\n");
}

void modifier_reservation(Reservation reservations[], int *count,
                         const Client clients[], int clients_count,
                         const Chambre chambres[], int chambres_count) {
    int id_a_modifier;
    int index_reservation = -1;
    
    printf("\n--- MODIFICATION D'UNE RESERVATION ---\n");
    printf("Entrez l'ID de la reservation a modifier: ");
    if (scanf("%d", &id_a_modifier) != 1) {
        printf("Erreur: ID invalide.\n");
        return;
    }
    
    index_reservation = trouver_reservation_par_id(reservations, *count, id_a_modifier);
    
    if (index_reservation == -1) {
        printf("Erreur: Reservation avec ID %d non trouvee.\n", id_a_modifier);
        return;
    }
    
    printf("\nReservation selectionnee:\n");
    printf("  ID: %d\n", reservations[index_reservation].id);
    printf("  Client ID: %d\n", reservations[index_reservation].client_id);
    printf("  Chambre: %d\n", reservations[index_reservation].chambre_numero);
    printf("  Dates: %s -> %s\n", reservations[index_reservation].date_debut,
           reservations[index_reservation].date_fin);
    printf("  Montant: %.2f EUR\n", reservations[index_reservation].montant);
    
    int choix;
    printf("\nQue voulez-vous modifier ? (1: Dates, 2: Chambre): ");
    if (scanf("%d", &choix) != 1) {
        printf("Erreur: Choix invalide.\n");
        return;
    }
    
    switch (choix) {
        case 1: {
            char nouvelle_date_debut[11], nouvelle_date_fin[11];
            printf("Nouvelle date de debut (actuelle: %s): ", reservations[index_reservation].date_debut);
            if (scanf("%10s", nouvelle_date_debut) != 1 || !valider_date(nouvelle_date_debut)) {
                printf("Erreur: Date invalide.\n");
                return;
            }
            
            printf("Nouvelle date de fin (actuelle: %s): ", reservations[index_reservation].date_fin);
            if (scanf("%10s", nouvelle_date_fin) != 1 || !valider_date(nouvelle_date_fin)) {
                printf("Erreur: Date invalide.\n");
                return;
            }
            
            if (comparer_dates(nouvelle_date_debut, nouvelle_date_fin) >= 0) {
                printf("Erreur: La date de fin doit etre apres la date de debut.\n");
                return;
            }
            
            /* Check availability with new dates */
            if (!chambre_disponible_dates(reservations, *count,
                                         reservations[index_reservation].chambre_numero,
                                         nouvelle_date_debut, nouvelle_date_fin,
                                         reservations[index_reservation].id)) {
                printf("Erreur: La chambre n'est pas disponible pour ces nouvelles dates.\n");
                return;
            }
            
            /* Update dates */
            strncpy(reservations[index_reservation].date_debut, nouvelle_date_debut,
                   sizeof(reservations[index_reservation].date_debut) - 1);
            reservations[index_reservation].date_debut[sizeof(reservations[index_reservation].date_debut) - 1] = '\0';
            strncpy(reservations[index_reservation].date_fin, nouvelle_date_fin,
                   sizeof(reservations[index_reservation].date_fin) - 1);
            reservations[index_reservation].date_fin[sizeof(reservations[index_reservation].date_fin) - 1] = '\0';
            
            /* Recalculate amount */
            int index_chambre = trouver_chambre_par_numero(chambres, chambres_count,
                                                          reservations[index_reservation].chambre_numero);
            if (index_chambre != -1) {
                int nuits = calculer_nuits(nouvelle_date_debut, nouvelle_date_fin);
                reservations[index_reservation].montant = calculer_montant_reservation(nuits,
                                                                                       chambres[index_chambre].prix);
            }
            break;
        }
        
        case 2: {
            int nouvelle_chambre;
            printf("Nouveau numero de chambre (actuel: %d): ",
                   reservations[index_reservation].chambre_numero);
            if (scanf("%d", &nouvelle_chambre) != 1) {
                printf("Erreur: Numero invalide.\n");
                return;
            }
            
            int index_chambre = trouver_chambre_par_numero(chambres, chambres_count, nouvelle_chambre);
            if (index_chambre == -1) {
                printf("Erreur: Chambre numero %d non trouvee.\n", nouvelle_chambre);
                return;
            }
            
            /* Check availability for new room */
            if (!chambre_disponible_dates(reservations, *count, nouvelle_chambre,
                                         reservations[index_reservation].date_debut,
                                         reservations[index_reservation].date_fin,
                                         reservations[index_reservation].id)) {
                printf("Erreur: La chambre %d n'est pas disponible pour ces dates.\n", nouvelle_chambre);
                return;
            }
            
            /* Update room and recalculate amount */
            reservations[index_reservation].chambre_numero = nouvelle_chambre;
            int nuits = calculer_nuits(reservations[index_reservation].date_debut,
                                      reservations[index_reservation].date_fin);
            reservations[index_reservation].montant = calculer_montant_reservation(nuits,
                                                                                   chambres[index_chambre].prix);
            break;
        }
        
        default:
            printf("Choix invalide. Aucune modification effectuee.\n");
            return;
    }
    
    /* Persist changes */
    sauvegarder_reservations(reservations, *count);
    printf("\nReservation %d modifiee et sauvegardee avec succes.\n", id_a_modifier);
}

void annuler_reservation(Reservation reservations[], int *count) {
    int id_a_annuler;
    int index_reservation = -1;
    
    printf("\n--- ANNULATION D'UNE RESERVATION ---\n");
    printf("Entrez l'ID de la reservation a annuler: ");
    if (scanf("%d", &id_a_annuler) != 1) {
        printf("Erreur: ID invalide.\n");
        return;
    }
    
    index_reservation = trouver_reservation_par_id(reservations, *count, id_a_annuler);
    
    if (index_reservation == -1) {
        printf("Erreur: Reservation avec ID %d non trouvee.\n", id_a_annuler);
        return;
    }
    
    printf("\nReservation a annuler:\n");
    printf("  ID: %d\n", reservations[index_reservation].id);
    printf("  Client ID: %d\n", reservations[index_reservation].client_id);
    printf("  Chambre: %d\n", reservations[index_reservation].chambre_numero);
    printf("  Dates: %s -> %s\n", reservations[index_reservation].date_debut,
           reservations[index_reservation].date_fin);
    printf("  Montant: %.2f EUR\n", reservations[index_reservation].montant);
    
    printf("\nConfirmez-vous l'annulation ? (o/n): ");
    char confirmation;
    scanf(" %c", &confirmation);
    
    if (confirmation != 'o' && confirmation != 'O') {
        printf("Annulation annulee.\n");
        return;
    }
    
    /* Remove from array */
    for (int i = index_reservation; i < (*count) - 1; i++) {
        reservations[i] = reservations[i + 1];
    }
    
    (*count)--;
    
    /* Persist changes */
    sauvegarder_reservations(reservations, *count);
    printf("\nReservation %d annulee et sauvegardee avec succes.\n", id_a_annuler);
}

