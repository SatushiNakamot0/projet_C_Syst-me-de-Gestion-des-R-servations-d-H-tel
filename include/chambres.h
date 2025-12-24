#ifndef CHAMBRES_H
#define CHAMBRES_H

#include "structures.h"

/* ============================================================================
 * ROOM MANAGEMENT MODULE
 * ============================================================================
 * Complete CRUD operations for hotel rooms with validation and persistence.
 * Follows the same pattern as clients.c for consistency.
 * ============================================================================ */

/* Add a new room - returns 0 on success, <0 on failure */
int chambre_ajouter(Chambre *chambres, int *count, const Chambre *nouvelle_chambre);

/* Modify an existing room - returns 0 on success, <0 on failure */
int chambre_modifier(Chambre *chambres, int count, const Chambre *modifiee);

/* Delete a room by room number - returns 0 on success, <0 on failure */
int chambre_supprimer(Chambre *chambres, int *count, int numero);

/* Find room by number - returns index or -1 if not found */
int trouver_chambre_par_numero(const Chambre chambres[], int count, int numero);

/* Check if room number already exists */
int chambre_numero_existe(const Chambre chambres[], int count, int numero);

/* Get available rooms count */
int compter_chambres_disponibles(const Chambre chambres[], int count);

/* Validate room data before save */
int valider_chambre(const Chambre *chambre, const Chambre chambres[], int count, int exclude_index);

#endif /* CHAMBRES_H */

