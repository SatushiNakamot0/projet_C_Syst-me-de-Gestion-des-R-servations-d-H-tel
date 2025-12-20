#ifndef CHAMBRES_H
#define CHAMBRES_H

#include "structures.h"

/* ============================================================================
 * ROOM MANAGEMENT MODULE
 * ============================================================================
 * Complete CRUD operations for hotel rooms with validation and persistence.
 * Follows the same pattern as clients.c for consistency.
 * ============================================================================ */

/* Add a new room - validates room number uniqueness */
void ajouter_chambre(Chambre chambres[], int *count);

/* Display all rooms in a formatted table */
void afficher_chambres(const Chambre chambres[], int count);

/* Modify an existing room by room number */
void modifier_chambre(Chambre chambres[], int count);

/* Delete a room by room number with confirmation */
void supprimer_chambre(Chambre chambres[], int *count);

/* Search rooms by type or number */
void rechercher_chambre(const Chambre chambres[], int count);

/* Find room by number - returns index or -1 if not found */
int trouver_chambre_par_numero(const Chambre chambres[], int count, int numero);

/* Check if room number already exists */
int chambre_numero_existe(const Chambre chambres[], int count, int numero);

/* Get available rooms count */
int compter_chambres_disponibles(const Chambre chambres[], int count);

/* Validate room data before save */
int valider_chambre(const Chambre *chambre, const Chambre chambres[], int count, int exclude_index);

#endif /* CHAMBRES_H */

