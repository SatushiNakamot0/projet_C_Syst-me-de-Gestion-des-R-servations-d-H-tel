#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reservations.h"
#include "structures.h"
#include "fichiers.h"
#include "clients.h"
#include "chambres.h"

#include "../include/debug.h"

/* Constants for validation */
#define MIN_YEAR 2020
#define MAX_YEAR 2100
#define DATE_LENGTH 11

/* ============================================================================
 * RESERVATION MANAGEMENT IMPLEMENTATION
 * ============================================================================
 * Enterprise-grade reservation engine with conflict detection, date validation,
 * and automatic calculations. Prevents double-booking and data corruption.
 * ============================================================================ */

/* Helper: Extract day, month, year from date string DD/MM/YYYY */
static void parser_date(const char *date, int *jour, int *mois, int *annee)
{
    if (!date || !jour || !mois || !annee)
        return;

    *jour = 0;
    *mois = 0;
    *annee = 0;

    if (strlen(date) != 10 || date[2] != '/' || date[5] != '/')
    {
        return;
    }

    *jour = atoi(date);
    *mois = atoi(date + 3);
    *annee = atoi(date + 6);
}

int valider_date(const char *date)
{
    if (!date || strlen(date) != 10)
    {
        LOG_ERROR("Format de date invalide: '%s'", date ? date : "NULL");
        return 0;
    }

    if (date[2] != '/' || date[5] != '/')
    {
        LOG_ERROR("Séparateurs de date invalides dans: '%s'", date);
        return 0;
    }

    int jour, mois, annee;
    parser_date(date, &jour, &mois, &annee);

    if (jour < 1 || jour > 31)
    {
        LOG_ERROR("Jour invalide: %d dans la date '%s'", jour, date);
        return 0;
    }
    if (mois < 1 || mois > 12)
    {
        LOG_ERROR("Mois invalide: %d dans la date '%s'", mois, date);
        return 0;
    }
    if (annee < MIN_YEAR || annee > MAX_YEAR)
    {
        LOG_ERROR("Année invalide: %d dans la date '%s' (doit être %d-%d)", annee, date, MIN_YEAR, MAX_YEAR);
        return 0;
    }

    /* Basic month day validation */
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (jour > jours_par_mois[mois - 1])
    {
        return 0;
    }

    return 1;
}

int comparer_dates(const char *date1, const char *date2)
{
    if (!date1 || !date2)
        return 0;

    int j1, m1, a1, j2, m2, a2;
    parser_date(date1, &j1, &m1, &a1);
    parser_date(date2, &j2, &m2, &a2);

    if (a1 < a2)
        return -1;
    if (a1 > a2)
        return 1;
    if (m1 < m2)
        return -1;
    if (m1 > m2)
        return 1;
    if (j1 < j2)
        return -1;
    if (j1 > j2)
        return 1;
    return 0;
}

int calculer_nuits(const char *date_debut, const char *date_fin)
{
    if (!valider_date(date_debut) || !valider_date(date_fin))
    {
        return -1;
    }

    int j1, m1, a1, j2, m2, a2;
    parser_date(date_debut, &j1, &m1, &a1);
    parser_date(date_fin, &j2, &m2, &a2);

    /* Simple calculation: convert to days since epoch (simplified) */
    /* For production, use proper date library, but this works for basic cases */
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    long jours1 = a1 * 365 + j1;
    for (int i = 0; i < m1 - 1; i++)
    {
        jours1 += jours_par_mois[i];
    }

    long jours2 = a2 * 365 + j2;
    for (int i = 0; i < m2 - 1; i++)
    {
        jours2 += jours_par_mois[i];
    }

    int nuits = (int)(jours2 - jours1);
    return nuits > 0 ? nuits : 0;
}

float calculer_montant_reservation(int nuits, float prix_nuit)
{
    if (nuits < 0 || prix_nuit < 0)
    {
        return 0.0f;
    }
    return nuits * prix_nuit;
}

int chambre_disponible_dates(const Reservation reservations[], int count,
                             int chambre_numero,
                             const char *date_debut, const char *date_fin,
                             int exclude_reservation_id)
{
    if (!valider_date(date_debut) || !valider_date(date_fin))
    {
        return 0; /* Invalid dates */
    }

    if (comparer_dates(date_debut, date_fin) >= 0)
    {
        return 0; /* Start date must be before end date */
    }

    /* Check all reservations for this room */
    for (int i = 0; i < count; i++)
    {
        /* Skip the reservation we're modifying */
        if (reservations[i].id == exclude_reservation_id)
        {
            continue;
        }

        /* Skip cancelled reservations */
        if (strcmp(reservations[i].statut, "ANNULEE") == 0)
        {
            continue;
        }

        /* Check if same room */
        if (reservations[i].chambre_numero != chambre_numero)
        {
            continue;
        }

        /* Check for date overlap */
        /* Overlap occurs if: (start1 < end2) && (start2 < end1) */
        if (comparer_dates(date_debut, reservations[i].date_fin) < 0 &&
            comparer_dates(reservations[i].date_debut, date_fin) < 0)
        {
            return 0; /* Conflict found */
        }
    }

    return 1; /* Available */
}

int trouver_reservation_par_id(const Reservation reservations[], int count, int id)
{
    for (int i = 0; i < count; i++)
    {
        if (reservations[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

const Reservation *obtenir_reservation(const Reservation reservations[], int count, int index)
{
    if (index < 0 || index >= count)
    {
        return NULL;
    }
    return &reservations[index];
}

/* ============================================================================
 * RESERVATION MANAGEMENT (PURE LOGIC)
 * ============================================================================ */

/* Helper: Generate next ID */
static int get_next_reservation_id(const Reservation reservations[], int count) {
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (reservations[i].id > max_id) {
            max_id = reservations[i].id;
        }
    }
    return max_id + 1;
}

/* Helper: Find room price */
static float get_room_price(int room_number, const Chambre *chambres, int chambres_count) {
    for (int i = 0; i < chambres_count; i++) {
        if (chambres[i].numero == room_number) {
            return chambres[i].prix;
        }
    }
    return 0.0f;
}

int reservation_ajouter(Reservation *reservations, int *count, const Reservation *input_res, const Chambre *chambres, int chambres_count) {
    if (*count >= MAX_RESERVATIONS) {
        LOG_ERROR("Erreur: Limite de réservations atteinte (%d)", MAX_RESERVATIONS);
        return -1; // Full
    }

    // Validate dates
    if (!valider_date(input_res->date_debut) || !valider_date(input_res->date_fin)) {
        LOG_ERROR("Dates invalides");
        return -2; // Invalid dates
    }

    if (comparer_dates(input_res->date_debut, input_res->date_fin) >= 0) {
        LOG_ERROR("Date de fin antérieure ou égale à la date de début");
        return -3; // End before start
    }

    // Check availability
    if (!chambre_disponible_dates(reservations, *count, input_res->chambre_numero, 
                                 input_res->date_debut, input_res->date_fin, -1)) {
        LOG_ERROR("Chambre %d non disponible", input_res->chambre_numero);
        return -4; // Not available
    }

    // Calculate nights and amount
    int nuits = calculer_nuits(input_res->date_debut, input_res->date_fin);
    float prix = get_room_price(input_res->chambre_numero, chambres, chambres_count);
    float montant = calculer_montant_reservation(nuits, prix);

    // Create final reservation
    Reservation *dest = &reservations[*count];
    *dest = *input_res;
    dest->id = get_next_reservation_id(reservations, *count);
    dest->montant = montant;
    strcpy(dest->statut, "ACTIVE");

    (*count)++;

    // Persist
    sauvegarder_reservations(reservations, *count);
    LOG_INFO("Réservation ajoutée: #%d (Client %d, Chambre %d)", dest->id, dest->client_id, dest->chambre_numero);
    return 0; // Success
}

int reservation_modifier(Reservation *reservations, int count, const Reservation *input_res, const Chambre *chambres, int chambres_count) {
    int index = trouver_reservation_par_id(reservations, count, input_res->id);
    if (index == -1) {
        return -1; // Not found
    }

    // Validate dates
    if (!valider_date(input_res->date_debut) || !valider_date(input_res->date_fin)) {
        return -2;
    }

    if (comparer_dates(input_res->date_debut, input_res->date_fin) >= 0) {
        return -3;
    }

    // Check availability (exclude self)
    if (!chambre_disponible_dates(reservations, count, input_res->chambre_numero, 
                                 input_res->date_debut, input_res->date_fin, input_res->id)) {
        return -4;
    }

    // Calculate nights and amount
    int nuits = calculer_nuits(input_res->date_debut, input_res->date_fin);
    float prix = get_room_price(input_res->chambre_numero, chambres, chambres_count);
    float montant = calculer_montant_reservation(nuits, prix);

    // Update
    reservations[index] = *input_res;
    reservations[index].montant = montant;

    // Persist
    sauvegarder_reservations(reservations, count);
    LOG_INFO("Réservation modifiée: #%d", input_res->id);
    return 0;
}

int reservation_annuler(Reservation *reservations, int *count, int id) {
    int index = trouver_reservation_par_id(reservations, *count, id);
    if (index == -1) {
        return -1; // Not found
    }

    // Soft delete: Change status to ANNULEE
    strcpy(reservations[index].statut, "ANNULEE");
    // (*count)--; // Do not remove from array

    // Persist
    sauvegarder_reservations(reservations, *count);
    LOG_INFO("Réservation annulée: #%d", id);
    return 0;
}
