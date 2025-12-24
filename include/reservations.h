#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include "structures.h"

/* ============================================================================
 * RESERVATION MANAGEMENT MODULE
 * ============================================================================
 * Complete reservation engine with conflict detection, date validation,
 * and automatic amount calculation. Prevents overlapping bookings.
 * ============================================================================ */

/* Add a new reservation - returns 0 on success, <0 on failure */
int reservation_ajouter(Reservation *reservations, int *count, const Reservation *input_res, const Chambre *chambres, int chambres_count);

/* Modify an existing reservation - returns 0 on success, <0 on failure */
int reservation_modifier(Reservation *reservations, int count, const Reservation *input_res, const Chambre *chambres, int chambres_count);

/* Cancel a reservation - returns 0 on success, <0 on failure */
int reservation_annuler(Reservation *reservations, int *count, int id);

/* Check if room is available for given date range */
int chambre_disponible_dates(const Reservation reservations[], int count,
                             int chambre_numero,
                             const char *date_debut, const char *date_fin,
                             int exclude_reservation_id);

/* Calculate number of nights between two dates */
int calculer_nuits(const char *date_debut, const char *date_fin);

/* Calculate total amount for a reservation */
float calculer_montant_reservation(int nuits, float prix_nuit);

/* Validate date format (DD/MM/YYYY) */
int valider_date(const char *date);

/* Compare two dates - returns -1 if date1 < date2, 0 if equal, 1 if date1 > date2 */
int comparer_dates(const char *date1, const char *date2);

/* Find reservation by ID */
int trouver_reservation_par_id(const Reservation reservations[], int count, int id);

/* Get reservation by index */
const Reservation* obtenir_reservation(const Reservation reservations[], int count, int index);

#endif /* RESERVATIONS_H */

